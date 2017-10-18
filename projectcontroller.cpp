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

    QDir projectsDir(projectsDirectory);
    projectsDir.setFilter(QDir::AllDirs);
    projectsNames = projectsDir.entryList();

    for (auto& name: projectsNames)
    {
        QDir projectDir(projectsDirectory + "\\" + name + "\\" + gameName + "_Data");
        if (!projectDir.exists() || projectDir.isEmpty())
            projectsNames.removeAll(name);
    }
}

