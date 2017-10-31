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
 * @details Обновление списка приложений. Именем проекта считается папка, в которой он находится
 */
void AppController::refreshAppsList()
{
    QDir projectsDir(appsDirectory);
    projectsDir.setFilter(QDir::AllDirs);
    appsNames = projectsDir.entryList();

    for (auto& name: appsNames)
    {
        if(!isGameDir(appsDirectory + QDir::separator() + name))
            appsNames.removeAll(name);
    }

    appsNames.removeAll("..");
}

/**
 * @brief AppController::prepareProject
 * @param index - номер проекта в списке приложений
 * @details Перемещение указанного приложения в директорию, откуда SimServer сможет его запустить
 */
bool AppController::prepareApp(uint index) const
{
    prepareDirectory(destinationDirectory, true);

    return copy_dir_recursive(appsDirectory + QDir::separator() + appsNames.at(index),
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

    prepareDirectory(defaultAppDirectory, true);

    return copy_dir_recursive(defaultAppDirectory, destinationDirectory);
}

/**
 * @brief AppController::loadProject
 * @details Копирование приложения в директорию проектов под указанным именем
 */
bool AppController::loadApp(QString appName, QString appDir)
{
    if (!isGameDir(appDir) || appName.trimmed().isEmpty())
        return false;

    if (appsNames.contains(appName))
        return false;

    QDir(appsDirectory).mkpath(appName);

    return copy_dir_recursive(appDir, appsDirectory + QDir::separator() + appName, false);
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

    return copy_dir_recursive(appDir, appsDirectory + QDir::separator() + appsNames.at(index));
}

/**
 * @brief AppController::isGameDir
 * @details Проверяет, содержит ли директория приложение для SimServer
 */
bool AppController::isGameDir(QString dirName)
{
    QDir gameDir(dirName + QDir::separator()
                 + gameName + "_Data");
    QFile executable(dirName + QDir::separator()
                     + gameName + ".exe");

    return gameDir.exists()    == true  &&
           gameDir.isEmpty()   == false &&
           executable.exists() == true;
}

/**
 * @brief AppController::copy_dir_recursive
 * @return Рекурсивно копирует всё содержимое одной директории в другую
 */
bool AppController::copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit) const
{
    QDir dir;
    dir.setPath(from_dir);

    from_dir += QDir::separator();
    to_dir += QDir::separator();

    for (const auto& copy_file : dir.entryList(QDir::Files))
    {
        QString from = from_dir + copy_file;
        QString to = to_dir + copy_file;

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
        QString to = to_dir + copy_dir;

        if (QDir(to_dir).mkpath(copy_dir) == false)
            return false;

        if (copy_dir_recursive(from, to, replace_on_conflit) == false)
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
        QMessageBox::warning(nullptr, "PlatMotionManager - Предупреждение", "Директория со студенческими приложениями не существует либо недоступна.\n"
                                                                            "Обратитесь к преподавателю");

    QFileInfo destinationDirectoryInfo(destinationDirectory);
    if (!destinationDirectoryInfo.exists() || !destinationDirectoryInfo.isWritable())
        QMessageBox::warning(nullptr, "PlatMotionManager - Предупреждение", "Директория, откуда SimServer запускает приложение, не существует либо недоступна.\n"
                                                                            "Обратитесь к преподавателю");

    QFileInfo defaultAppDirectoryInfo(defaultAppDirectory);
    if (!defaultAppDirectoryInfo.exists() ||
        !defaultAppDirectoryInfo.isReadable() ||
        !isGameDir(defaultAppDirectory))
        QMessageBox::warning(nullptr, "PlatMotionManager - Предупреждение", "Указана неверная директория для приложения по умолчанию.\n"
                                                                            "Обратитесь к преподавателю");
    QFileInfo sendEnterDirectoryInfo(sendEnterDirectory);
    QFileInfo sendEnterExeInfo(sendEnterDirectory + QDir::separator() + sendEnterName);
    if (!sendEnterDirectoryInfo.exists() ||
        !sendEnterDirectoryInfo.isReadable() ||
        !sendEnterExeInfo.exists() ||
        !sendEnterDirectoryInfo.isReadable())
        QMessageBox::warning(nullptr, "PlatMotionManager - Предупреждение", "Указана неверная директория для SendEnter.\n"
                                                                            "Обратитесь к преподавателю");
}