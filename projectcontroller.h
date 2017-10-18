#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QtCore/QtCore>
#include <memory>

class ProjectController
{
public:
    ProjectController();

private:
    const QString settingsFileName = "settings.ini";
    const QString gameName = "PlatMotionGame";

    std::unique_ptr<QSettings> settings;

    QString projectsDirectory;
    QString destinationDirectory;
    QStringList projectsNames;


    void initialize();
    QStringList getProjectsNames();
    bool prepareProject(uint index);
};

#endif // PROJECTCONTROLLER_H
