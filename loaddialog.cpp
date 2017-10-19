#include "loaddialog.h"
#include "ui_loaddialog.h"
#include "projectcontroller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

LoadDialog::LoadDialog(bool add, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog),
    add(add)
{
    ui->setupUi(this);

    if (add)
        setWindowTitle("Добавить проект");
    else
    {
        setWindowTitle("Обновить проект");
        ui->namePanel->hide();
    }
}

LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::on_browsePB_clicked()
{
    QString projectDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "%USERPROFILE%",
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (projectDir.isEmpty())
        return;

    ui->projectDirLE->setText(projectDir);
}

QString LoadDialog::getProjectName() const
{
    return ui->projectNameLE->text();
}

QString LoadDialog::getProjectDir() const
{
    return ui->projectDirLE->text();
}

void LoadDialog::on_buttonBox_accepted()
{
    if (add && ui->projectNameLE->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Имя проекта пустое");
        return;
    }

    if (ui->projectDirLE->text().isEmpty())
    {
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Путь к приложению пустой");
        return;
    }

    if (!ProjectController::isGameDir(ui->projectDirLE->text()))
    {
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Указанная директория не содержит файлы приложения для SimServer");
        return;
    }

    accept();
}
