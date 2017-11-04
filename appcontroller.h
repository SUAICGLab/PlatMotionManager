#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QtCore/QtCore>
#include <memory>

/**
 * @brief The AppController class
 * @details Содержит все функции, относящиеся к управлению приложениями для SimServer.
 * Предполагается, что в appsDirectory находятся директории
 * с приложениями студентов. AppController позволяет скопировать нужное приложение
 * в destinationDirectory, откуда SimServer сможет его запустить. Кроме того,
 * на случай, если что-то пойдёт не так, можно восстановить приложение из defaultDirectory.
 */
class AppController
{
public:
    AppController();

    const QStringList getAppsNames() const { return appsNames; }
    const QString getAppsDirectory() const { return appsDirectory; }
    bool prepareApp(uint index) const;
    void refreshAppsList();
    bool restoreDefaultApp() const;
    bool loadApp(QString appName, QString appDir);
    bool reloadApp(uint index, QString appDir);

    static bool isGameDir(QString dirName);

private:
    const QString settingsFileName = "settings.ini";
    const QString sendEnterName = "SendEnter.exe";
    static const QString gameName;

    std::unique_ptr<QSettings> settings;
    QString appsDirectory;
    QString destinationDirectory;
    QString defaultAppDirectory;
    QString sendEnterDirectory;
    QStringList appsNames;

    void initialize();
    bool copyDirRecursive(QString from_dir, QString to_dir, bool replace_on_conflit = true) const;
    void prepareDirectory(QString directory, bool copySimServer = false) const;
    void checkSettings();
};

#endif // APPCONTROLLER_H
