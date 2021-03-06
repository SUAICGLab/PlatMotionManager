#include "loaddialog.h"
#include "ui_loaddialog.h"
#include "appcontroller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

LoadDialog::LoadDialog(Action action, QWidget* parent) :
    QDialog(parent),
    ui(std::make_unique<Ui::LoadDialog>()),
    add(action == Action::Add)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    if (add)
        setWindowTitle("Добавить приложение");
    else
    {
        setWindowTitle("Обновить приложение");
        ui->namePanel->hide();
    }
}

LoadDialog::~LoadDialog()
{
}

void LoadDialog::on_browsePB_clicked()
{
    QString appDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "%USERPROFILE%",
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (appDir.isEmpty())
        return;

    ui->projectDirLE->setText(appDir);
}

QString LoadDialog::getAppName() const
{
    return ui->projectNameLE->text();
}

QString LoadDialog::getAppDir() const
{
    return ui->projectDirLE->text();
}

void LoadDialog::setAppName(QString name)
{
    ui->projectNameLE->setText(name);
}

void LoadDialog::on_buttonBox_accepted()
{
    if (add)
    {
        if (getAppName().trimmed().isEmpty())
        {
            QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Имя приложения пустое");
            return;
        }

        if (getAppName().trimmed().contains(QRegularExpression("[\\\\/]"))) // слеш и обратный слеш
        {
            QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Имя приложения содержит недопустимые символы");
            return;
        }
    }

    if (getAppDir().isEmpty())
    {
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Путь к приложению пустой");
        return;
    }

    if (!AppController::isGameDir(ui->projectDirLE->text()))
    {
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Указанная директория не содержит файлы приложения для SimServer");
        return;
    }

    if (!add && QMessageBox::question(this, "PlatMotionManager - Предупреждение",
                                      QString("Вы хотите обновить приложение <b>%1</b>.<br>"
                                              "Все файлы предыдущей версии приложения могут быть потеряны!<br>"
                                              "Продолжить?").arg(getAppName())
                                      ) == QMessageBox::No)
    {
        return;
    }

    accept();
}
