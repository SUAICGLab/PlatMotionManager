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
    bool restoreDefaultGame() const;
    const QString& getProjectsDirectory() const {return projectsDirectory; }

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
    bool isGameDir(QString dirName) const;
};

#endif // PROJECTCONTROLLER_H
