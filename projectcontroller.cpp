#include "projectcontroller.h"
#include <QMessageBox>

ProjectController::ProjectController()
{
    settings = std::make_unique<QSettings>(settingsFileName, QSettings::IniFormat);
    initialize();
}

void ProjectController::initialize()
{
    settings->beginGroup("Main");
    projectsDirectory    = settings->value("projects-directory").toString();
    destinationDirectory = settings->value("game-directory").toString();
    defaultGameDirectory = settings->value("default-game-directory").toString();
    settings->endGroup();

    if (!QFile(settingsFileName).exists())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Файл настроек \"settings.ini\" не существует.\n"
                                                                             "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    if (projectsDirectory.isEmpty() || destinationDirectory.isEmpty()|| defaultGameDirectory.isEmpty())
    {
        QMessageBox::critical(nullptr, "PlatMotionManager - Предупреждение", "Неверный формат файла настроек.\n"
                                                                             "Один или несколько параметров не указаны.\n"
                                                                             "Продолжение работы невозможно. Обратитесь к преподавателю");
        exit(EXIT_FAILURE);
    }

    refreshProjectsList();
}

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

bool ProjectController::prepareProject(uint index) const
{
    QFile(destinationDirectory + QDir::separator() + gameName + ".exe").remove();
    QDir(destinationDirectory + QDir::separator() + gameName + "_Data").removeRecursively();

    return copy_dir_recursive(projectsDirectory + QDir::separator() + projectsNames.at(index),
                              destinationDirectory, true);
}

bool ProjectController::restoreDefaultGame() const
{
    if (!isGameDir(defaultGameDirectory))
            return false;

    QFile(destinationDirectory + QDir::separator() + gameName + ".exe").remove();
    QDir(destinationDirectory + QDir::separator() + gameName + "_Data").removeRecursively();

    return copy_dir_recursive(defaultGameDirectory, destinationDirectory, true);
}

bool ProjectController::isGameDir(QString dirName) const
{
    QDir gameDir(dirName + QDir::separator()
                 + gameName + "_Data");
    QFile executable(dirName + QDir::separator()
                     + gameName + ".exe");

    return gameDir.exists()    == true  &&
           gameDir.isEmpty()   == false &&
           executable.exists() == true;
}

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
