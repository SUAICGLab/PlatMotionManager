#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QtCore/QtCore>
#include <memory>

class ProjectController
{
public:
    ProjectController();

    const QStringList& getProjectsNames() const {return projectsNames; }
    bool prepareProject(uint index) const;
    void refreshList();

private:
    const QString settingsFileName = "settings.ini";
    const QString gameName = "PlatMotionGame";

    std::unique_ptr<QSettings> settings;

    QString projectsDirectory;
    QString destinationDirectory;
    QString defaultGameDirectory;
    QStringList projectsNames;


    void initialize();
    bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit) const;
    bool isGameDir(QString dirName);
};

#endif // PROJECTCONTROLLER_H
