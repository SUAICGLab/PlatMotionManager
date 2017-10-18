#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>
#include <QMessageBox>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->projectsTW->resizeColumnsToContents();
    ui->projectsTW->setSelectionMode(QAbstractItemView::SingleSelection);
    projectController = std::make_unique<ProjectController>();
    fillTable();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_defaultPB_clicked()
{

}

void MainWidget::on_launchPB_clicked()
{
    if (!ui->projectsTW->selectionModel()->selectedRows(0).isEmpty())
    {
        if (!projectController->prepareProject(ui->projectsTW->selectionModel()->selectedRows(0).at(0).row()))
            QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Не могу скопировать файл");
        else
            QMessageBox::information(this, "PlatMotionManager", "Готово!");
    }
}

void MainWidget::on_refreshProjectsPB_clicked()
{
    fillTable();
}

void MainWidget::on_aboutPB_clicked()
{

}

void MainWidget::fillTable()
{
    projectController->refreshList();
    ui->projectsTW->clearContents();
    ui->projectsTW->setRowCount(projectController->getProjectsNames().size());

    for (auto row = 0; row < projectController->getProjectsNames().size(); row++)
        ui->projectsTW->setItem(row, 0,
                                new QTableWidgetItem(projectController->getProjectsNames().at(row))
                                );

    ui->projectsTW->update();
}
