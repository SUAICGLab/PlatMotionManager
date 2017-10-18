#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->projectsTW->resizeColumnsToContents();
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
    ui->projectsTW->clearContents();
    ui->projectsTW->setRowCount(projectController->getProjectsNames().size());

    for (auto row = 0; row < projectController->getProjectsNames().size(); row++)
        ui->projectsTW->setItem(row, 0,
                                new QTableWidgetItem(projectController->getProjectsNames().at(row))
                                );

    ui->projectsTW->update();
}
