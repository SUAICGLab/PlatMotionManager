#include "projectcontroller.h"

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
    settings->endGroup();

    refreshList();
}

void ProjectController::refreshList()
{
    QDir projectsDir(projectsDirectory);
    projectsDir.setFilter(QDir::AllDirs);
    projectsNames = projectsDir.entryList();

    for (auto& name: projectsNames)
    {
        QDir projectDir(projectsDirectory + QDir::separator()
                        + name + QDir::separator()
                        + gameName + "_Data");
        QFile executable(projectsDirectory + QDir::separator()
                         + name + QDir::separator()
                         + gameName + ".exe");
        if (projectDir.exists() == false || projectDir.isEmpty()
            || executable.exists() == false)
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

/*static bool copyRecursively(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}*/

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
