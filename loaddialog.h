#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>

namespace Ui {
class LoadDialog;
}

/**
 * @brief The LoadDialog class
 * @details Диалог выбора проекта
 */
class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadDialog(bool add = true, QWidget *parent = 0);
    ~LoadDialog();
    void setProjectName(QString name);

    QString getProjectName() const;
    QString getProjectDir() const;

private slots:
    void on_browsePB_clicked();
    void on_buttonBox_accepted();

private:
    Ui::LoadDialog *ui;
    bool add;
};

#endif // LOADDIALOG_H
