#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->projectsTW->resizeColumnsToContents();

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
