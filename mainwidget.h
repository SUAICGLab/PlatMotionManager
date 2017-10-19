#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "projectcontroller.h"

#include <QWidget>
#include <memory>

namespace Ui {
class MainWidget;
}

/**
 * @brief The MainWidget class
 * Реализует пользовательский интерфейс для управления возможностями ProjectController.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void on_defaultPB_clicked();
    void on_launchPB_clicked();
    void on_refreshProjectsPB_clicked();
    void on_aboutPB_clicked();

private:
    Ui::MainWidget *ui;
    std::unique_ptr<ProjectController> projectController;

    const QString version = "0.1";

    void fillTable();
};
#endif // MAINWIDGET_H
