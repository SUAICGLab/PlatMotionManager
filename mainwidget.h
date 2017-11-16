#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "appcontroller.h"
#include "loaddialog.h"

#include <QWidget>
#include <memory>

namespace Ui {
class MainWidget;
}

/**
 * @brief The MainWidget class
 * @details Реализует пользовательский интерфейс для управления возможностями ProjectController.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

private slots:
    void on_defaultPB_clicked();
    void on_launchPB_clicked();
    void on_refreshProjectsPB_clicked();
    void on_aboutPB_clicked();
    void on_loadPB_clicked();
    void on_reloadPB_clicked();

private:
    Ui::MainWidget *ui;
    std::unique_ptr<AppController> appController;

    const QString version = "0.3.2";

    void fillTable();
};
#endif // MAINWIDGET_H
