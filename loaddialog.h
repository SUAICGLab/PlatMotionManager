#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>
#include <memory>

namespace Ui {
class LoadDialog;
}

/**
 * @brief The LoadDialog class
 * @details Диалог выбора приложения
 */
class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Action
    {
        Add,
        Update
    };

    explicit LoadDialog(Action action = Action::Add, QWidget* parent = nullptr);
    ~LoadDialog();

    void setAppName(QString name);
    QString getAppName() const;
    QString getAppDir() const;

private slots:
    void on_browsePB_clicked();
    void on_buttonBox_accepted();

private:
    std::unique_ptr<Ui::LoadDialog> ui;
    bool add;
};

#endif // LOADDIALOG_H
