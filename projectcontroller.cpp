#include "projectcontroller.h"
#include <QMessageBox>

const QString ProjectController::gameName = "PlatMotionGame";

ProjectController::ProjectController()
{
    settings = std::make_unique<QSettings>(settingsFileName, QSettings::IniFormat);
    initialize();
}

/**
 * @brief ProjectController::initialize
 * @details Загрузка настроек из файла и чтение списка проектов
 */
void ProjectController::initialize()
{
    settings->beginGroup("Main");
    projectsDirectory    = settings->value("projects-directory").toString();
    destinationDirectory = settings->value("game-directory").toString();
    defaultGameDirectory = settings->value("default-game-directory").toString();
    sendEnterDirectory   = settings->value("send-enter-directory").toString();
    settings->endGroup();

    if (!QFile(settingsFileName).exists())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Ошибка", "Файл настроек \"settings.ini\" не существует.\n"
                                                                     "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    if (projectsDirectory.isEmpty() || destinationDirectory.isEmpty() || defaultGameDirectory.isEmpty() || sendEnterDirectory.isEmpty())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Ошибка", "Неверный формат файла настроек.\n"
                                                                      "Один или несколько параметров не указаны.\n"
                                                                      "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    refreshProjectsList();
}

/**
 * @brief ProjectController::refreshProjectsList
 * @details Обновление списка проектов. Именем проекта считается папка, в которой он находится
 */
void ProjectController::refreshProjectsList()
{
    QDir projectsDir(projectsDirectory);
    projectsDir.setFilter(QDir::AllDirs);
    projectsNames = projectsDir.entryList();

    for (auto& name: projectsNames)
    {
        if(!isGameDir(projectsDirectory + QDir::separator() + name))
            projectsNames.removeAll(name);
    }

    projectsNames.removeAll("..");
}

/**
 * @brief ProjectController::prepareProject
 * @param index - номер проекта в списке проектов
 * @details Перемещение указанного проекта в директорию, откуда SimServer сможет его запустить
 */
bool ProjectController::prepareProject(uint index) const
{
    prepareDestinationDirectory();

    return copy_dir_recursive(projectsDirectory + QDir::separator() + projectsNames.at(index),
                              destinationDirectory, true);
}

/**
 * @brief ProjectController::restoreDefaultProject
 * @details Перемещение проекта по умолчанию в директорию, откуда SimServer сможет его запустить
 */
bool ProjectController::restoreDefaultProject() const
{
    if (!isGameDir(defaultGameDirectory))
            return false;

    prepareDestinationDirectory();

    return copy_dir_recursive(defaultGameDirectory, destinationDirectory);
}

/**
 * @brief ProjectController::loadProject
 * @details Копирование проект в директорию проектов под указанным именем
 */
bool ProjectController::loadProject(QString projectName, QString projectDir)
{
    if (!isGameDir(projectDir) || projectName.trimmed().isEmpty())
        return false;

    if (projectsNames.contains(projectName))
        return false;

    QDir(projectsDirectory).mkpath(projectName);

    return copy_dir_recursive(projectDir, projectsDirectory + QDir::separator() + projectName, false);
}

/**
 * @brief ProjectController::reloadProject
 * @details Обновление указанного проекта (т.е. удаление всех файлов проекта и копирование новых из указанной директории)
 */
bool ProjectController::reloadProject(uint index, QString projectDir)
{
    if (!isGameDir(projectDir))
        return false;

    QFile(projectsDirectory + QDir::separator() + projectsNames.at(index) + QDir::separator() + gameName + ".exe").remove();
    QDir(projectsDirectory + QDir::separator() + projectsNames.at(index) + QDir::separator() + gameName + "_Data").removeRecursively();

    return copy_dir_recursive(projectDir, projectsDirectory + QDir::separator() + projectsNames.at(index));
}

/**
 * @brief ProjectController::isGameDir
 * @details Проверяет, содержит ли директория приложение (проект) для SimServer
 */
bool ProjectController::isGameDir(QString dirName)
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
 * @brief ProjectController::copy_dir_recursive
 * @return Рекурсивно копирует всё содержимое одной директории в другую
 */
bool ProjectController::copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit) const
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
 * @brief ProjectController::prepareDestinationDirectory
 * @details Удаляет destinationDirectory вместе со всем содержимым и создаёт новую,
 *          пустую директорию с тем же именем, а затем копирует туда SendEnter
 */
void ProjectController::prepareDestinationDirectory() const
{
    QDir destinationDir(destinationDirectory);
    QString destinationDirName = destinationDir.dirName();
    destinationDir.removeRecursively();
    destinationDir.cdUp();
    destinationDir.mkpath(destinationDirName);
    QFile::copy(sendEnterDirectory   + QDir::separator() + sendEnterName,
                destinationDirectory + QDir::separator() + sendEnterName);
}
