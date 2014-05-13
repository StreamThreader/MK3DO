#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidget>
#include <QMainWindow>
#include "settingsdialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    
private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_action_6_triggered();

    void on_action_triggered();

    void on_action_3_triggered();

    void on_action_2_triggered();

    void on_action_4_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_action_7_triggered();

    void on_action_5_triggered();

    void on_action_8_triggered();

    void on_action_9_triggered();

    void on_action_10_triggered();

    void on_action_11_triggered();

    void on_action_12_triggered();

    void on_action_13_triggered();

    void on_actionGit_triggered();

    void on_pushButton_4_clicked();

    void on_actionEnglish_triggered();

    void on_action_14_triggered();

    void on_action_15_triggered();

    void on_action_settings_menu_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
