#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QtCore/QtCore>
#include <memory>

/**
 * @brief The ProjectController class
 * Содержит все функции, относящиеся к управлению приложениями для SimServer.
 * Предполагается, что в projectsDirectory находятся директории
 * с приложениями студентов. ProjectController позволяет скопировать нужное приложение
 * в destinationDirectory, откуда SimServer сможет его запустить. Кроме того,
 * на случай, если что-то пойдёт не так, можно восстановить приложение из defaultDirectory.
 */
class ProjectController
{
public:
    ProjectController();

    const QStringList& getProjectsNames() const { return projectsNames; }
    bool prepareProject(uint index) const;
    void refreshProjectsList();
    bool restoreDefaultProject() const;
    bool loadProject(QString projectName, QString projectDir);
    bool reloadProject(uint index, QString projectDir);
    const QString& getProjectsDirectory() const {return projectsDirectory; }


    static bool isGameDir(QString dirName);

private:
    const QString settingsFileName = "settings.ini";
    static const QString gameName;

    std::unique_ptr<QSettings> settings;

    QString projectsDirectory;
    QString destinationDirectory;
    QString defaultGameDirectory;
    QStringList projectsNames;

    void initialize();
    bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit) const;
};

#endif // PROJECTCONTROLLER_H
