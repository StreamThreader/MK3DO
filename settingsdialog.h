#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);

    int fntsz();

    void fontsetsize(int a);

    ~SettingsDialog();
    
private slots:

    void on_buttonBox_accepted();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
