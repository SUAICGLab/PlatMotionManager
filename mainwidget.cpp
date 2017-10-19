#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QMessageBox>
#include <QFileDialog>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->projectsTW->resizeColumnsToContents();
    ui->projectsTW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->projectsTW->setEditTriggers(QTableWidget::NoEditTriggers);

    projectController = std::make_unique<ProjectController>();
    fillTable();

    connect(ui->projectsTW, &QTableWidget::itemSelectionChanged,
            [this]()
            {
                if (ui->projectsTW->selectedItems().isEmpty())
                {
                    ui->reloadPB->setDisabled(true);
                    ui->launchPB->setDisabled(true);
                }
                else
                {
                    ui->reloadPB->setEnabled(true);
                    ui->launchPB->setEnabled(true);
                }
            }
    );
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_defaultPB_clicked()
{
    if (!projectController->restoreDefaultProject())
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Не могу восстановить приложение по умолчанию.\n"
                                                                         "Обратитесь к преподавателю");
    else
        QMessageBox::information(this, "PlatMotionManager", "Готово!");
}

void MainWidget::on_launchPB_clicked()
{
    if (ui->projectsTW->selectionModel()->selectedRows(0).isEmpty())
        return;

    uint selectedRow = ui->projectsTW->selectionModel()->selectedRows(0).at(0).row();

    if (!projectController->prepareProject(selectedRow))
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Не могу скопировать приложение");
    else
        QMessageBox::information(this, "PlatMotionManager", "Готово!");

}

void MainWidget::on_refreshProjectsPB_clicked()
{
    fillTable();
}

void MainWidget::on_loadPB_clicked()
{
    auto loadDilog = std::make_unique<LoadDialog>();
    loadDilog->exec();

    if (loadDilog->result() == QDialog::Rejected)
        return;

    if (!projectController->loadProject(loadDilog->getProjectName(), loadDilog->getProjectDir()))
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Не могу добавить приложение");
    else
        QMessageBox::information(this, "PlatMotionManager", "Готово!");

    fillTable();
}

void MainWidget::on_reloadPB_clicked()
{
    if (ui->projectsTW->selectionModel()->selectedRows(0).isEmpty())
        return;

    uint selectedRow = ui->projectsTW->selectionModel()->selectedRows(0).at(0).row();
    auto loadDilog = std::make_unique<LoadDialog>(false);
    loadDilog->setProjectName(projectController->getProjectsNames().at(selectedRow));
    loadDilog->exec();

    if (loadDilog->result() == QDialog::Rejected)
        return;

    if (!projectController->reloadProject(selectedRow,
                                          loadDilog->getProjectDir()))
        QMessageBox::warning(this, "PlatMotionManager - Предупреждение", "Не могу обновить приложение");
    else
        QMessageBox::information(this, "PlatMotionManager", "Готово!");
}

void MainWidget::on_aboutPB_clicked()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("PlatMotionManager - О программе");
    aboutBox.setIconPixmap(QPixmap("://resources/aboutIcon.png"));
    aboutBox.setText(tr("<strong>PlatMotionManager</strong> v%1  - это приложение, позволяющее управлять запуском "
                        "ваших приложений на Unity через SimSever").arg(version));
    aboutBox.setInformativeText("<p>" + tr("<strong>Как добавить своё приложение:</strong>"
                                           "<ol>"
                                           "<li>Соберите ваше приложение под именем PlatMotionGame</li>"
                                           "<li>Нажмите кнопку \"Добавить приложение\", укажите путь к собранным файлам и имя приложения.</li>"
                                           "<li>Выделите ваше прилолжение и нажмите \"Подготовить к запуску\". </li>"
                                           "</ol>"
                                           "После этого вы можете запустить проект через SimServer как обычно, согласно инструкции запуска "
                                           "(вы можете попросить её у преподавателя).<br><br>"
                                           "Вы также можете обновить выбранное вами приложение или восстановить приложение по умолчанию.<br><br>")
                                .arg(QDir(projectController->getProjectsDirectory()).absolutePath()) +
                                  tr("Репозиторий: ") +
                                "<a href=https://github.com/SUAICGLab/PlatMotionManager>https://github.com/SUAICGLab/PlatMotionManager</a><br>");
    aboutBox.exec();
}

void MainWidget::fillTable()
{
    projectController->refreshProjectsList();
    ui->projectsTW->clearContents();
    ui->projectsTW->setRowCount(projectController->getProjectsNames().size());

    for (auto row = 0; row < projectController->getProjectsNames().size(); row++)
        ui->projectsTW->setItem(row, 0,
                                new QTableWidgetItem(projectController->getProjectsNames().at(row))
                                );

    ui->projectsTW->update();
}
