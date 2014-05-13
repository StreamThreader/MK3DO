#include "settingsdialog.h"
#include "ui_settingsdialog.h"

int fonticsize;


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{

}

// Select font size
void SettingsDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    fonticsize = arg1.toInt();
}

int SettingsDialog::fntsz()
{
    return fonticsize;
}

// Font set size
void SettingsDialog::fontsetsize(int a)
{
    int rescom = ui->comboBox->findText(QString::number(a));
    if(rescom != -1)
    {
        ui->comboBox->setCurrentIndex(rescom);
    }
}
