#include "appcontroller.h"
#include <QMessageBox>

const QString AppController::gameName = "PlatMotionGame";

AppController::AppController()
{
    settings = std::make_unique<QSettings>(settingsFileName, QSettings::IniFormat);
    initialize();
}

/**
 * @brief AppController::initialize
 * @details Загрузка настроек из файла и чтение списка приложений
 */
void AppController::initialize()
{
    settings->beginGroup("Main");
    appsDirectory        = settings->value("apps-directory").toString();
    destinationDirectory = settings->value("simserver-app-directory").toString();
    defaultAppDirectory  = settings->value("default-app-directory").toString();
    sendEnterDirectory   = settings->value("send-enter-directory").toString();
    settings->endGroup();

    checkSettings();
    refreshAppsList();
}

/**
 * @brief AppController::refreshProjectsList
 * @details Обновление списка приложений. Именем приложения считается папка, в которой оно находится
 */
void AppController::refreshAppsList()
{
    QDir projectsDir(appsDirectory);
    appsNames = projectsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (auto& name: appsNames)
    {
        if (!isGameDir(appsDirectory + QDir::separator() + name))
            appsNames.removeAll(name);
    }
}

/**
 * @brief AppController::prepareProject
 * @param index - номер приложения в списке приложений (appsNames)
 * @details Перемещение указанного приложения в директорию, откуда SimServer сможет его запустить
 */
bool AppController::prepareApp(uint index) const
{
    if (!isGameDir(appsDirectory + QDir::separator() + appsNames.at(index)))
        return false;

    prepareDirectory(destinationDirectory, true);

    return copyDirRecursive(appsDirectory + QDir::separator() + appsNames.at(index),
                            destinationDirectory, true);
}

/**
 * @brief AppController::restoreDefaultProject
 * @details Перемещение приложения по умолчанию в директорию, откуда SimServer сможет его запустить
 */
bool AppController::restoreDefaultApp() const
{
    if (!isGameDir(defaultAppDirectory))
            return false;

    prepareDirectory(destinationDirectory, true);

    return copyDirRecursive(defaultAppDirectory, destinationDirectory, true);
}

/**
 * @brief AppController::loadProject
 * @details Копирование приложения в директорию проектов под указанным именем
 */
bool AppController::loadApp(QString appName, QString appDir)
{
    if (!isGameDir(appDir) || appName.trimmed().isEmpty())
        return false;

    if (appsNames.contains(appName) || QDir(appsDirectory + QDir::separator() + appName).exists())
        return false;

    QDir(appsDirectory).mkpath(appName);

    return copyDirRecursive(appDir, appsDirectory + QDir::separator() + appName, false);
}

/**
 * @brief AppController::reloadProject
 * @details Обновление указанного приложения (т.е. удаление всех файлов приложения и копирование новых из указанной директории)
 */
bool AppController::reloadApp(uint index, QString appDir)
{
    if (!isGameDir(appDir))
        return false;

    prepareDirectory(appsDirectory + QDir::separator() + appsNames.at(index));

    return copyDirRecursive(appDir, appsDirectory + QDir::separator() + appsNames.at(index));
}

/**
 * @brief AppController::isGameDir
 * @details Проверяет, содержит ли директория приложение для SimServer
 */
bool AppController::isGameDir(QString dirName)
{
    QDir  gameDir(   dirName + QDir::separator() + gameName + "_Data");
    QFile executable(dirName + QDir::separator() + gameName + ".exe");

    return gameDir.exists()    == true  &&
           gameDir.isEmpty()   == false &&
           executable.exists() == true;
}

/**
 * @brief AppController::copyDirRecursive
 * @return Рекурсивно копирует всё содержимое одной директории в другую
 */
bool AppController::copyDirRecursive(QString from_dir, QString to_dir, bool replace_on_conflit) const
{
    QDir dir;
    dir.setPath(from_dir);

    from_dir += QDir::separator();
    to_dir   += QDir::separator();

    for (const auto& copy_file : dir.entryList(QDir::Files))
    {
        QString from = from_dir + copy_file;
        QString to   = to_dir   + copy_file;

        if (QFile::exists(to))
        {
            if (replace_on_conflit)
            {
                if (QFile::remove(to) == false)
                    return false;
            }
            else
            {
                continue;
            }
        }

        if (QFile::copy(from, to) == false)
            return false;
    }

    for (const auto& copy_dir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString from = from_dir + copy_dir;
        QString to   = to_dir   + copy_dir;

        if (QDir(to_dir).mkpath(copy_dir) == false)
            return false;

        if (copyDirRecursive(from, to, replace_on_conflit) == false)
            return false;
    }

    return true;
}

/**
 * @brief AppController::prepareDirectory
 * @details Удаляет указанную директорию вместе со всем содержимым и создаёт новую,
 *          пустую директорию с тем же именем и если нужно, копирует туда SendEnter
 */
void AppController::prepareDirectory(QString directory, bool copySimServer) const
{
    QDir dir(directory);
    QString dirName = dir.dirName();
    dir.removeRecursively();
    dir.cdUp();
    dir.mkpath(dirName);

    if (copySimServer)
        QFile::copy(sendEnterDirectory + QDir::separator() + sendEnterName,
                    directory          + QDir::separator() + sendEnterName);
}

/**
 * @brief AppController::checkSettings
 * @details Проверяет корректность настроек в файле настроек
 */
void AppController::checkSettings()
{
    if (!QFile(settingsFileName).exists())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Ошибка", "Файл настроек \"settings.ini\" не существует.\n"
                                                                     "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    if (appsDirectory.isEmpty() || destinationDirectory.isEmpty() || defaultAppDirectory.isEmpty() || sendEnterDirectory.isEmpty())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Ошибка", "Неверный формат файла настроек.\n"
                                                                      "Один или несколько параметров не указаны.\n"
                                                                      "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    QFileInfo appsDirectoryInfo(appsDirectory);

    if (!appsDirectoryInfo.exists() || !appsDirectoryInfo.isWritable())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Директория со студенческими приложениями не существует либо недоступна.\n"
                                                                             "Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    QFileInfo destinationDirectoryInfo(destinationDirectory);
    if (!destinationDirectoryInfo.exists() || !destinationDirectoryInfo.isWritable())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Директория, откуда SimServer запускает приложение, не существует либо недоступна.\n"
                                                                             "Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    QFileInfo defaultAppDirectoryInfo(defaultAppDirectory);
    if (!defaultAppDirectoryInfo.exists() ||
        !defaultAppDirectoryInfo.isReadable() ||
        !isGameDir(defaultAppDirectory))
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Указана неверная директория для приложения по умолчанию.\n"
                                                                             "Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    QFileInfo sendEnterDirectoryInfo(sendEnterDirectory);
    QFileInfo sendEnterExeInfo(sendEnterDirectory + QDir::separator() + sendEnterName);
    if (!sendEnterDirectoryInfo.exists() ||
        !sendEnterDirectoryInfo.isReadable() ||
        !sendEnterExeInfo.exists() ||
        !sendEnterDirectoryInfo.isReadable())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Указана неверная директория для SendEnter.\n"
                                                                             "Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }
}
