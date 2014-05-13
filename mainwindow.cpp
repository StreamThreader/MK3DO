#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QFile>
#include <QList>
#include <QDate>
#include "exiv2/exiv2.hpp"


int countgam=0;                     // How many games found in sqlite
int jpgcount;                       // How many jpg files found
QStringList listjpg;                // list of jpg files for game
int jpgcstate=0;                    // Current show jpg count position
QString cutit;                      // Current item name
QDir dir;                           // Current dir for game
QString cdr;                        // Current dir
QString categdb = "Games";          // Category of DB (table name)
QString dbname = "3dodb.sqlite";    // Data Base file name
QString langrow = "textdata";       // Default language row
QString langcreator = "creator";    // Lang creator
QString langganre = "ganre";        // Lang ganre
QString usrcommnt;                  // User comment
int fontsize;                       // Set fontsize



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QFile dbfile(dbname);
    if(!dbfile.exists()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("Не найден файл базы данных.\nСоздать пустую базу?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Yes:
            {
                // Connect to DataBase
                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(dbname);
                if (!db.open()) {
                        qDebug() << db.lastError().text();
                        exit(1);
                    }

                 // Create Table for Games
                 QSqlQuery query("CREATE TABLE Games ( \
                    name TEXT,\
                    textdata TEXT,\
                    textdata_enu TEXT, \
                    rating INTEGER,\
                    emustat INTEGER,\
                    year INTEGER,\
                    creator TEXT, \
                    creator_enu TEXT, \
                    ganre TEXT, \
                    ganre_enu TEXT, \
                    gamers INTEGER, \
                    complexity INTEGER \
                   )");

                 // Create Table for Hardware
                 QSqlQuery querya("CREATE TABLE Hardware (\
                    name TEXT, \
                    textdata TEXT, \
                    textdata_enu TEXT, \
                    rating TEXT, \
                    creator TEXT, \
                    creator_enu TEXT \
                   )");

                 // Create Table for Software
                 QSqlQuery queryb("CREATE TABLE Software ( \
                    name TEXT, \
                    textdata TEXT, \
                    textdata_enu TEXT, \
                    rating TEXT, \
                    creator TEXT, \
                    creator_enu TEXT \
                   )");

                 // Create Table for info
                 QSqlQuery queryc("CREATE TABLE info (name TEXT, value TEXT)");

                 QSqlQuery querd("INSERT INTO info (name, value) VALUES ('version', '102')");

                 QSqlQuery quere("INSERT INTO info (name, value) VALUES ('owner', '(Не зарегистрированно)')");

                 QSqlQuery queryy("INSERT INTO info (name, value) VALUES ('fontsize', '9')");

                 break;
           }
           case QMessageBox::No:
           {
                ui->pushButton->setDisabled(true);
                ui->pushButton_2->setDisabled(true);
                ui->pushButton_3->setDisabled(true);
                ui->pushButton_4->setDisabled(true);
                ui->action->setDisabled(true);
                ui->action_2->setDisabled(true);
                ui->action_3->setDisabled(true);
                ui->action_9->setDisabled(true);
                ui->action_10->setDisabled(true);
                ui->action_11->setDisabled(true);
                ui->action_12->setDisabled(true);
                ui->action_15->setDisabled(true);
                return;
           }
        }
    } else {
        // Connect to DataBase
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbname);
        if (!db.open()) {
                qDebug() << db.lastError().text();
                exit(1);
            }
    }

    // Language settings
    ui->actionEnglish->setEnabled(true);
    ui->action_14->setDisabled(true);

    // Get version from DB
    QSqlQuery queryx("SELECT value FROM info WHERE name='version'");
    while (queryx.next())
    {
        // If version of DB is Wrong
        QString verstxt = queryx.value(0).toString();
        ui->label_14->setText(verstxt);
        if (verstxt != "102") {
            QMessageBox::critical(this, "MK3DO", "База данных не подходит", QMessageBox::Ok);
            ui->action_3->setDisabled(true);
            ui->action_2->setDisabled(true);
            ui->action_9->setDisabled(true);
            ui->pushButton->setDisabled(true);
            ui->pushButton_2->setDisabled(true);
            ui->pushButton_3->setDisabled(true);
            ui->action_10->setDisabled(true);
            ui->action_11->setDisabled(true);
            ui->action_12->setDisabled(true);
            ui->action->setDisabled(true);
            return;
        }
    }

    // Get list of all games from DB
    QSqlQuery query("SELECT name FROM "+categdb+" ORDER BY name");
    while (query.next())
    {
        QString textdata = query.value(0).toString();
        ui->listWidget->addItem(textdata);
        countgam++;
    }

    QSqlQuery queryy("SELECT value FROM info WHERE name='fontsize'");
    while (queryy.next())
    {
        fontsize = queryy.value(0).toInt();
        ui->plainTextEdit->setFont(QFont ("", fontsize));
    }

    // Counter of games in DB
    ui->label->setText(QString("Записей в базе: %1").arg(countgam));

    ui->listWidget->setCurrentRow(0);

    ui->action_10->setDisabled(true);
    ui->action_11->setEnabled(true);
    ui->action_12->setEnabled(true);

    if(countgam == 0) {
        ui->action_3->setDisabled(true);
        ui->action_2->setDisabled(true);
        ui->action_9->setDisabled(true);
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
    }
    else
    {
        ui->action_3->setEnabled(true);
        ui->action_2->setEnabled(true);
        ui->action_9->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
    }

    // Set title-counter
    // Get current date
    QDate cdat(QDate::currentDate());

    int toct=276;

    int lastday=0;

    QString wtitle;

    // Check LeapYear or not
    if(QDate::isLeapYear(cdat.year())) {
        toct = 277;
    }

    int curday(cdat.dayOfYear());

    if(curday < toct) {
        lastday = (toct - curday);
    }

    if(curday > toct) {
        lastday = (curday + toct);
    }

    wtitle=("осталось "+QString::number(lastday)+" дней до дня 3DO-шника");

    // If current day is 3 october
    if(curday == toct) {
        int age(cdat.year() - 1993);
        wtitle=("сегодня 3DO исполняется: "+QString::number(age)+" лет. C праздником!");
    }

    this->setWindowTitle("MK3DO - ("+wtitle+")");

    ui->exif2label->clear();

    // Current game dir
    dir = "DB/"+cutit;

    // Get count of jpg files
    QStringList filters;
    filters << "*.jpg";
    dir.setNameFilters(filters);
    listjpg = dir.entryList();
    listjpg.sort();
    jpgcount = listjpg.count();

    // Get exif from first record if jpg exist
    if(jpgcount > 0 ) {
        cdr = QDir::currentPath();
        QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
        QFile qfff(targetjpg);
        if(!qfff.exists()) return;
        std::string pthjpg = targetjpg.toStdString();
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pthjpg);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::Exifdatum& tag = exifData["Exif.Photo.UserComment"];
        std::string usrcomm = tag.toString();
        usrcomm.erase(0, 18);
        usrcommnt = QString::fromStdString(usrcomm);
        ui->exif2label->clear();
        ui->exif2label->setText(usrcommnt);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


// If Change game in list
void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    // Check currnet item if no zero
    if(current == 0) return;

    if(countgam == 0) {
        ui->action_3->setDisabled(true);
        ui->action_2->setDisabled(true);
        ui->action_9->setDisabled(true);
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
    } else {
        ui->action_3->setEnabled(true);
        ui->action_2->setEnabled(true);
        ui->action_9->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
    }

    ui->exif2label->clear();
    ui->plainTextEdit->clear();

    // Get name of current item
    cutit = ui->listWidget->currentItem()->text();

    // Current game dir
    dir = "DB/"+cutit;

    // Get name
    QSqlQuery query;
    query.prepare("SELECT name FROM "+categdb+" WHERE name =?");
    query.bindValue(0, cutit);
    query.exec();
    while (query.next()) {
        QString textdata = query.value(0).toString();
        ui->lineEdit_3->clear();
        ui->lineEdit_3->insert(textdata);
    }

    // Get text
    QSqlQuery querya;
    querya.prepare("SELECT "+langrow+" FROM "+categdb+" WHERE name =?");
    querya.bindValue(0, cutit);
    querya.exec();
    while (querya.next()) {
        QString textdata = querya.value(0).toString();
        ui->plainTextEdit->insertPlainText(textdata);
    }

    // Get rating
    QSqlQuery queryb;
    queryb.prepare("SELECT rating FROM "+categdb+" WHERE name =?");
    queryb.bindValue(0, cutit);
    queryb.exec();
    while (queryb.next()) {
        int textdata = queryb.value(0).toInt();
        ui->spinBox_3->clear();
        ui->spinBox_3->setValue(textdata);
    }

    // Get creator
    QSqlQuery querye;
    querye.prepare("SELECT "+langcreator+" FROM "+categdb+" WHERE name =?");
    querye.bindValue(0, cutit);
    querye.exec();
    while (querye.next()) {
        QString textdata = querye.value(0).toString();
        ui->lineEdit_2->clear();
        ui->lineEdit_2->insert(textdata);
    }


    // If category not a Games, switch off no needed queries
    if(categdb == "Games") {
        // Get emustat
        QSqlQuery queryc;
        queryc.prepare("SELECT emustat FROM "+categdb+" WHERE name =?");
        queryc.bindValue(0, cutit);
        queryc.exec();
        while (queryc.next()) {
            int textdata = queryc.value(0).toInt();
            ui->spinBox_2->clear();
            ui->spinBox_2->setValue(textdata);
        }

        // Get year
        QSqlQuery queryd;
        queryd.prepare("SELECT year FROM "+categdb+" WHERE name =?");
        queryd.bindValue(0, cutit);
        queryd.exec();
        while (queryd.next()) {
            int textdata = queryd.value(0).toInt();
            ui->spinBox->clear();
            ui->spinBox->setValue(textdata);
        }

        // Get ganre
        QSqlQuery queryf;
        queryf.prepare("SELECT "+langganre+" FROM "+categdb+" WHERE name =?");
        queryf.bindValue(0, cutit);
        queryf.exec();
        while (queryf.next()) {
            QString textdata = queryf.value(0).toString();
            ui->lineEdit->clear();
            ui->lineEdit->insert(textdata);
        }

        // Get gamers
        QSqlQuery queryg;
        queryg.prepare("SELECT gamers FROM "+categdb+" WHERE name =?");
        queryg.bindValue(0, cutit);
        queryg.exec();
        while (queryg.next()) {
            int textdata = queryg.value(0).toInt();
            ui->spinBox_4->clear();
            ui->spinBox_4->setValue(textdata);
        }

        // Get complexity
        QSqlQuery queryh;
        queryh.prepare("SELECT complexity FROM "+categdb+" WHERE name =?");
        queryh.bindValue(0, cutit);
        queryh.exec();
        while (queryh.next()) {
            int textdata = queryh.value(0).toInt();
            ui->spinBox_5->clear();
            ui->spinBox_5->setValue(textdata);
        }
    }

    QGraphicsScene *scene = new QGraphicsScene;

    // If dir with jpg for current item not exist
    if(!dir.exists()) {
        QPixmap pictr("DB/Other/noise.jpg");
        scene->addPixmap(pictr);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->show();
        ui->pushButton_3->setDisabled(true);
        ui->pushButton_4->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton->setDisabled(true);
        return;
    } else {
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton->setEnabled(true);
    }

    // Get count of jpg files
    QStringList filters;
    filters << "*.jpg";
    dir.setNameFilters(filters);
    listjpg = dir.entryList();
    listjpg.sort();
    jpgcount = listjpg.count();

    if(jpgcount > 0 ) {
        ui->pushButton->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton_2->setDisabled(true);
    } else {
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
        ui->pushButton_4->setDisabled(true);
        QPixmap pictr("DB/Other/noise.jpg");
        scene->addPixmap(pictr);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->show();
        ui->pushButton->setEnabled(true);
        return;
    }

    // Get geometry from GraphicsView
    int width = ui->graphicsView->geometry().width();
    int height = ui->graphicsView->geometry().height();

    // New scene with 0x0
    QGraphicsScene *viewScene = new QGraphicsScene(QRectF(0, 0, width, height), 0);
    // QPixmap JPG file
    QPixmap pictr("DB/"+cutit+"/"+listjpg[0]);
    // QGraphics Pixmap Item
    QGraphicsPixmapItem *item;
    // Add JPG to scene with scaled state
    item = viewScene->addPixmap(pictr.scaled(QSize((int)viewScene->width(), (int)viewScene->height()),Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // GrapgicsView Fit in View
    ui->graphicsView->fitInView(QRectF(0, 0, width, height),Qt::KeepAspectRatio);
    // set scene fot GraphicsView
    ui->graphicsView->setScene(viewScene);
    // Show result
    ui->graphicsView->show();
    // Reset counters
    jpgcstate=0;

    cdr = QDir::currentPath();
    QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
    QFile qfff(targetjpg);
    if(!qfff.exists()) return;
    std::string pthjpg = targetjpg.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pthjpg);
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    Exiv2::Exifdatum& tag = exifData["Exif.Photo.UserComment"];
    std::string usrcomm = tag.toString();
    usrcomm.erase(0, 18);
    usrcommnt = QString::fromStdString(usrcomm);
    ui->exif2label->clear();
    ui->exif2label->setText(usrcommnt);
}

// Exit menu
void MainWindow::on_action_6_triggered()
{
    exit(0);
}

// Add new record
void MainWindow::on_action_triggered()
{
    bool ok;
    QString text;
    text = QInputDialog::getText(this, "Добавление новой записи", "Название:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            // Check if already exist
            QSqlQuery querry;
            querry.prepare("SELECT * FROM "+categdb+" WHERE name=?");
            querry.bindValue(0, text);
            querry.exec();
            while (querry.next()) {
                QMessageBox::warning(this, "MK3DO", "Запись под названием '"+text+"' уже есть.", QMessageBox::Ok);
                return;
            }

            // If this is a geme cateory
            if(categdb == "Games") {
                QString sqlrowname = text;
                QString sqlrowtextdata = "Описания пока нет";
                QString sqlrowtextdata_enu = "Empty";
                QString sqlrowrating = "0";
                QString sqlemustat = "0";
                QString sqlrowyear = "1991";
                QString sqlrowcreator = "Пусто";
                QString sqlrowcreator_enu = "Empty";
                QString sqlrowganre = "Пусто";
                QString sqlrowganre_enu = "Empty";
                QString sqlrowgamers = "0";
                QString sqlrowcomplexity = "0";

                QSqlQuery query;
                query.prepare("INSERT INTO "+categdb+" (name, textdata, textdata_enu, rating, emustat, year, creator, creator_enu, \
                    ganre, ganre_enu, gamers, complexity) \
                        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                query.bindValue(0, sqlrowname);
                query.bindValue(1, sqlrowtextdata);
                query.bindValue(2, sqlrowtextdata_enu);
                query.bindValue(3, sqlrowrating);
                query.bindValue(4, sqlemustat);
                query.bindValue(5, sqlrowyear);
                query.bindValue(6, sqlrowcreator);
                query.bindValue(7, sqlrowcreator_enu);
                query.bindValue(8, sqlrowganre);
                query.bindValue(9, sqlrowganre_enu);
                query.bindValue(10, sqlrowgamers);
                query.bindValue(11, sqlrowcomplexity);
                query.exec();
            } else {
                QString sqlrowname = text;
                QString sqlrowtextdata = "Описания пока нет";
                QString sqlrowtextdata_enu = "Empty";
                QString sqlrowrating = "0";
                QString sqlrowcreator = "Пусто";
                QString sqlrowcreator_enu = "Empty";

                QSqlQuery query;
                query.prepare("INSERT INTO "+categdb+" \
                    (name, textdata, textdata_enu, rating, creator, creator_enu) \
                        VALUES (?, ?, ?, ?, ?, ?)");
                query.bindValue(0, sqlrowname);
                query.bindValue(1, sqlrowtextdata);
                query.bindValue(2, sqlrowtextdata_enu);
                query.bindValue(3, sqlrowrating);
                query.bindValue(4, sqlrowcreator);
                query.bindValue(5, sqlrowcreator_enu);
                query.exec();
            }

            // Update list
            ui->listWidget->addItem(text);
            countgam++;
            ui->label->setText(QString("Записей в базе: %1").arg(countgam));
            ui->listWidget->sortItems();
        }
}


// Remove game
void MainWindow::on_action_3_triggered()
{
    QString cutit = ui->listWidget->currentItem()->text();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Вы действительно хотите удалить запись '"+cutit+"' ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::Yes:
            // Query delete
            QSqlQuery query;
            query.prepare("DELETE FROM "+categdb+" WHERE name=?");
            query.bindValue(0, cutit);
            query.exec();
            // Remove deleted item from list
            QListWidgetItem *widget = ui->listWidget->currentItem();
            delete widget;
            --countgam;
            ui->label->setText(QString("Записей в базе: %1").arg(countgam));

            // If no left any items
            if(countgam == 0) {
                ui->action_3->setDisabled(true);
                ui->action_2->setDisabled(true);
                ui->action_9->setDisabled(true);
                ui->pushButton->setDisabled(true);
                ui->pushButton_2->setDisabled(true);
                ui->pushButton_3->setDisabled(true);
            } else {
                ui->action_3->setEnabled(true);
                ui->action_2->setEnabled(true);
                ui->action_9->setEnabled(true);
                ui->pushButton->setEnabled(true);
                ui->pushButton_2->setEnabled(true);
                ui->pushButton_3->setEnabled(true);
            }

            QString dirchk=".";
            QDir dirgam(QString("DB/"+cutit));
            if(dirgam.exists()) {
                dirchk = "\nОбратите внимание, что дректорию с изображениями программа не тронула.";
            }

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("Запись "+cutit+" удалена"+dirchk+"");
            msgBox.exec();
    }
}


// Add changes
void MainWindow::on_action_2_triggered()
{
    QString cutit = ui->listWidget->currentItem()->text();
    QString sqlrowname = "";

    if(categdb == "Games") {
        sqlrowname = ui->lineEdit_3->text();
        QString sqlrowrating = ui->spinBox_3->text();
        QString sqlemustat = ui->spinBox_2->text();
        QString sqlrowyear = ui->spinBox->text();
        QString sqlrowcreator = ui->lineEdit_2->text();
        QString sqlrowganre = ui->lineEdit->text();
        QString sqlrowgamers = ui->spinBox_4->text();
        QString sqlrowcomplexity = ui->spinBox_5->text();
        QString sqlrowtextdata = ui->plainTextEdit->toPlainText();

        QSqlQuery query;
        query.prepare("SELECT rowid FROM "+categdb+" WHERE name=?");
        query.bindValue(0, cutit);
        query.exec();
        while (query.next()) {
            QString stringrowid = query.value(0).toString();
            QSqlQuery querya;
            querya.prepare("UPDATE "+categdb+" \
                SET name=?, "+langrow+"=?, rating=?, emustat=?, year=?, "+langcreator+"=?, "+langganre+"=?, gamers=?, complexity=? \
                    WHERE rowid='"+stringrowid+"'");
            querya.bindValue(0, sqlrowname);
            querya.bindValue(1, sqlrowtextdata);
            querya.bindValue(2, sqlrowrating);
            querya.bindValue(3, sqlemustat);
            querya.bindValue(4, sqlrowyear);
            querya.bindValue(5, sqlrowcreator);
            querya.bindValue(6, sqlrowganre);
            querya.bindValue(7, sqlrowgamers);
            querya.bindValue(8, sqlrowcomplexity);
            querya.exec();
        }
    } else {
        sqlrowname = ui->lineEdit_3->text();
        QString sqlrowtextdata = ui->plainTextEdit->toPlainText();
        QString sqlrowrating = ui->spinBox_3->text();
        QString sqlrowcreator = ui->lineEdit_2->text();

        QSqlQuery query;
        query.prepare("SELECT rowid FROM "+categdb+" WHERE name=?");
        query.bindValue(0, cutit);
        query.exec();
        while (query.next()) {
            QString stringrowid = query.value(0).toString();
            QSqlQuery querya("UPDATE "+categdb+" \
                SET name=?, "+langrow+"=?, rating=?, "+langcreator+"=? \
                    WHERE rowid='"+stringrowid+"'");
            querya.bindValue(0, sqlrowname);
            querya.bindValue(1, sqlrowtextdata);
            querya.bindValue(2, sqlrowrating);
            querya.bindValue(3, sqlrowcreator);
            querya.exec();
        }
    }

    // Rename current item in list
    QListWidgetItem* itm(ui->listWidget->currentItem());
    itm->setText(sqlrowname);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Изменения в "+cutit+" сохранены.");
    msgBox.exec();
}

//About
void MainWindow::on_action_4_triggered()
{
    QString owner;

    QSqlQuery query("SELECT value FROM info WHERE name='owner'");
    while (query.next()) {
        owner = query.value(0).toString();
    }

    QMessageBox::about(this, "О Программе", \
                   " Версия программы MK3DO: 1.010 \
                   \nАвтор: Олег Деордиев \
                   \nSkype: StreamThreader \
                   \n \
                   \nВладелец данной копии: "+owner+" \
                   \n \
                   \n В проекте участвовали: \
                   \nAltmer \
                   \nNamtarius \
                   \nDoom_sun \
                   \nDenstan \
                   \nTotaku \
                   \nAliast \
                   \nSilentman \
                   \nCpuHanter \
                   \nAlex_Punch \
                   \nAspyd \
                   \nPantera \
                   \nVersus \
                   \nIvpet87 \
                   \nStarefan \
                   \nSasha \
                   \nMnemonic \
                   \nYuri Hristev(samael) \
                   \n_MOHAPX_ \
                   \nSlimm Jimm \
                   \nOmikron \
                   \nSashapont \
                   \nAtla \
                   \nLopez \
                   \nBlack Jack \
                   \nLe Taon \
                   \nHellraiser \
                   \n \
                    ");
}

// Next jpeg
void MainWindow::on_pushButton_clicked()
{
    if(jpgcstate < (jpgcount - 1)) {
        jpgcstate++;
        // Get geometry from GraphicsView
        int width = ui->graphicsView->geometry().width();
        int height = ui->graphicsView->geometry().height();

        // New scene with 0x0
        QGraphicsScene *viewScene = new QGraphicsScene(QRectF(0, 0, width, height), 0);
        // QPixmap JPG file
        QPixmap pictr("DB/"+cutit+"/"+listjpg[jpgcstate]);
        // QGraphics Pixmap Item
        QGraphicsPixmapItem *item;
        // Add JPG to scene with scaled state
        item = viewScene->addPixmap(pictr.scaled(QSize((int)viewScene->width(), (int)viewScene->height()),Qt::KeepAspectRatio, Qt::SmoothTransformation));
        // GrapgicsView Fit in View
        ui->graphicsView->fitInView(QRectF(0, 0, width, height),Qt::KeepAspectRatio);
        // set scene fot GraphicsView
        ui->graphicsView->setScene(viewScene);
        // Show result
        ui->graphicsView->show();

        ui->pushButton_2->setEnabled(true);
        if(jpgcstate == (jpgcount - 1)) ui->pushButton->setDisabled(true);

        cdr = QDir::currentPath();
        QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
        QFile qfff(targetjpg);
        if(!qfff.exists()) return;
        std::string pthjpg = targetjpg.toStdString();
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pthjpg);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::Exifdatum& tag = exifData["Exif.Photo.UserComment"];
        std::string usrcomm = tag.toString();
        usrcomm.erase(0, 18);
        usrcommnt = QString::fromStdString(usrcomm);
        ui->exif2label->clear();
        ui->exif2label->setText(usrcommnt);
    } else {
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setEnabled(true);
    }
}

// Previous jpeg
void MainWindow::on_pushButton_2_clicked()
{
    if(jpgcstate > 0) {
        jpgcstate--;

        // Get geometry from GraphicsView
        int width = ui->graphicsView->geometry().width();
        int height = ui->graphicsView->geometry().height();

        // New scene with 0x0
        QGraphicsScene *viewScene = new QGraphicsScene(QRectF(0, 0, width, height), 0);
        // QPixmap JPG file
        QPixmap pictr("DB/"+cutit+"/"+listjpg[jpgcstate]);
        // QGraphics Pixmap Item
        QGraphicsPixmapItem *item;
        // Add JPG to scene with scaled state
        item = viewScene->addPixmap(pictr.scaled(QSize((int)viewScene->width(), (int)viewScene->height()),Qt::KeepAspectRatio, Qt::SmoothTransformation));
        // GrapgicsView Fit in View
        ui->graphicsView->fitInView(QRectF(0, 0, width, height),Qt::KeepAspectRatio);
        // set scene fot GraphicsView
        ui->graphicsView->setScene(viewScene);
        // Show result
        ui->graphicsView->show();

        ui->pushButton->setEnabled(true);
        if(jpgcstate == 0) ui->pushButton_2->setDisabled(true);

        cdr = QDir::currentPath();
        QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
        QFile qfff(targetjpg);
        if(!qfff.exists()) return;
        std::string pthjpg = targetjpg.toStdString();
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pthjpg);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::Exifdatum& tag = exifData["Exif.Photo.UserComment"];
        std::string usrcomm = tag.toString();
        usrcomm.erase(0, 18);
        usrcommnt = QString::fromStdString(usrcomm);
        ui->exif2label->clear();
        ui->exif2label->setText(usrcommnt);
    } else {
        ui->pushButton_2->setDisabled(true);
        ui->pushButton->setEnabled(true);
    }
}

// Open jpeg
void MainWindow::on_pushButton_3_clicked()
{
    cdr = QDir::currentPath();
    QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
    QUrl jpgpath = QUrl::fromLocalFile(targetjpg);
    QDesktopServices::openUrl(jpgpath);
}

// SendMail
void MainWindow::on_action_7_triggered()
{
    QDesktopServices::openUrl(QUrl("mailto:admin@ifconfig.com.ua"));
}

// Check updates
void MainWindow::on_action_5_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.arts-union.ru/smf/index.php?topic=184.0"));
}

// Got ot forum
void MainWindow::on_action_8_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.arts-union.ru/smf/index.php?topic=131.0"));
}

// Save as txt
void MainWindow::on_action_9_triggered()
{
    QString plantxt = ui->plainTextEdit->toPlainText();
    QString savflname = QFileDialog::getSaveFileName(this, "Save File", cutit+".txt", "*.txt (*.txt)");
    QFile savfile(savflname);
    if (!savfile.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&savfile);
    out << plantxt;
    savfile.close();
}

// Games DB
void MainWindow::on_action_10_triggered()
{
    // New table
    categdb = "Games";

    ui->plainTextEdit->clear();
    ui->spinBox->clear();
    ui->spinBox->setEnabled(true);
    ui->spinBox_2->clear();
    ui->spinBox_2->setEnabled(true);
    ui->spinBox_4->clear();
    ui->spinBox_4->setEnabled(true);
    ui->spinBox_5->clear();
    ui->spinBox_5->setEnabled(true);
    ui->lineEdit->clear();
    ui->lineEdit->setEnabled(true);
    ui->action_11->setEnabled(true);
    ui->action_10->setDisabled(true);
    ui->action_12->setEnabled(true);
    ui->listWidget->clear();

    // Reset counter
    countgam=0;

    // Get names of new table
    QSqlQuery query("SELECT name FROM "+categdb+" ORDER BY name");
    while (query.next()) {
        QString textdata = query.value(0).toString();
        ui->listWidget->addItem(textdata);
        countgam++;
    }

    // Counter of games in DB
    ui->label->setText(QString("Записей в базе: %1").arg(countgam));

    ui->listWidget->setCurrentRow(0);
}

// Software DB
void MainWindow::on_action_11_triggered()
{
    // New table
    categdb = "Software";

    ui->plainTextEdit->clear();
    ui->spinBox->clear();
    ui->spinBox->setDisabled(true);
    ui->spinBox_2->clear();
    ui->spinBox_2->setDisabled(true);
    ui->spinBox_4->clear();
    ui->spinBox_4->setDisabled(true);
    ui->spinBox_5->clear();
    ui->spinBox_5->setDisabled(true);
    ui->lineEdit->clear();
    ui->lineEdit->setDisabled(true);
    ui->action_11->setDisabled(true);
    ui->action_10->setEnabled(true);
    ui->action_12->setEnabled(true);
    ui->listWidget->clear();

    // Reset counter
    countgam=0;

    // Get names of new table
    QSqlQuery query("SELECT name FROM "+categdb+" ORDER BY name");
    while (query.next()) {
        QString textdata = query.value(0).toString();
        ui->listWidget->addItem(textdata);
        countgam++;
    }

    // Counter of games in DB
    ui->label->setText(QString("Записей в базе: %1").arg(countgam));

    ui->listWidget->setCurrentRow(0);
}

// Hardware DB
void MainWindow::on_action_12_triggered()
{
    // New table
    categdb = "Hardware";

    ui->plainTextEdit->clear();
    ui->spinBox->clear();
    ui->spinBox->setDisabled(true);
    ui->spinBox_2->clear();
    ui->spinBox_2->setDisabled(true);
    ui->spinBox_4->clear();
    ui->spinBox_4->setDisabled(true);
    ui->spinBox_5->clear();
    ui->spinBox_5->setDisabled(true);
    ui->lineEdit->clear();
    ui->lineEdit->setDisabled(true);
    ui->action_11->setEnabled(true);
    ui->action_10->setEnabled(true);
    ui->action_12->setDisabled(true);
    ui->listWidget->clear();

    // Reset counter
    countgam=0;

    // Get names of new table
    QSqlQuery query("SELECT name FROM "+categdb+" ORDER BY name");
    while (query.next()) {
        QString textdata = query.value(0).toString();
        ui->listWidget->addItem(textdata);
        countgam++;
    }

    // Counter of games in DB
    ui->label->setText(QString("Записей в базе: %1").arg(countgam));

    ui->listWidget->setCurrentRow(0);
}

// Register
void MainWindow::on_action_13_triggered()
{
    QString owner;
    bool ok;
    QString text=owner;

    QSqlQuery query("SELECT value FROM info WHERE name='owner'");
    while (query.next()) {
        owner = query.value(0).toString();
    }

    text = QInputDialog::getText(this, "Регистрация", "Ваше имя:", QLineEdit::Normal, owner, &ok);
    if (ok && !text.isEmpty()) {
       QSqlQuery querya;
       querya.prepare("UPDATE info SET name='owner', value=? WHERE name='owner'");
       querya.bindValue(0, text);
       querya.exec();
    }
}

// GIT Repository
void MainWindow::on_actionGit_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/StreamThreader/MK3DO"));
}

// Language Changed to ENU
void MainWindow::on_actionEnglish_triggered()
{
    ui->action_14->setEnabled(true);
    ui->actionEnglish->setDisabled(true);

    // Change enviroment variable
    langrow = "textdata_enu";
    langcreator="creator_enu";
    langganre="ganre_enu";

    //Update viewport
    ui->plainTextEdit->clear();
    // Get text
    QSqlQuery querya;
    querya.prepare("SELECT "+langrow+" FROM "+categdb+" WHERE name =?");
    querya.bindValue(0, cutit);
    querya.exec();
    while (querya.next())
    {
        QString textdata = querya.value(0).toString();
        ui->plainTextEdit->insertPlainText(textdata);
    }

    // Get creator
    QSqlQuery querye;
    querye.prepare("SELECT "+langcreator+" FROM "+categdb+" WHERE name =?");
    querye.bindValue(0, cutit);
    querye.exec();
    while (querye.next()) {
        QString textdata = querye.value(0).toString();
        ui->lineEdit_2->clear();
        ui->lineEdit_2->insert(textdata);
    }

    // If category not a Games, switch off no needed queries
    if(categdb == "Games") {
        // Get ganre
        QSqlQuery queryf;
        queryf.prepare("SELECT "+langganre+" FROM "+categdb+" WHERE name =?");
        queryf.bindValue(0, cutit);
        queryf.exec();
        while (queryf.next()) {
            QString textdata = queryf.value(0).toString();
            ui->lineEdit->clear();
            ui->lineEdit->insert(textdata);
        }
    }
}

// Language Changed to RU
void MainWindow::on_action_14_triggered()
{
    ui->actionEnglish->setEnabled(true);
    ui->action_14->setDisabled(true);

    // Change enviroment variable
    langrow = "textdata";
    langcreator="creator";
    langganre="ganre";


    //Update viewport
    ui->plainTextEdit->clear();
    // Get text
    QSqlQuery querya;
    querya.prepare("SELECT "+langrow+" FROM "+categdb+" WHERE name =?");
    querya.bindValue(0, cutit);
    querya.exec();
    while (querya.next()) {
        QString textdata = querya.value(0).toString();
        ui->plainTextEdit->insertPlainText(textdata);
    }

    // Get creator
    QSqlQuery querye;
    querye.prepare("SELECT "+langcreator+" FROM "+categdb+" WHERE name =?");
    querya.bindValue(0, cutit);
    querya.exec();
    while (querye.next()) {
        QString textdata = querye.value(0).toString();
        ui->lineEdit_2->clear();
        ui->lineEdit_2->insert(textdata);
    }

    // If category not a Games, switch off no needed queries
    if(categdb == "Games") {
        // Get ganre
        QSqlQuery queryf;
        queryf.prepare("SELECT "+langganre+" FROM "+categdb+" WHERE name =?");
        queryf.bindValue(0, cutit);
        queryf.exec();
        while (queryf.next()) {
            QString textdata = queryf.value(0).toString();
            ui->lineEdit->clear();
            ui->lineEdit->insert(textdata);
        }
    }
}


// Load from txt file
void MainWindow::on_action_15_triggered()
{
    QString exportfilename = QFileDialog::getOpenFileName(this, tr("Open File"),"DB/",tr("txt (*.txt)"));
    if(exportfilename == "" ) return;
    ui->plainTextEdit->clear();
    QFile exportfile(exportfilename);
    if (!exportfile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QString contentxt = exportfile.readAll();
    ui->plainTextEdit->setPlainText(contentxt);
    exportfile.close();
}


// EXIF edit button
void MainWindow::on_pushButton_4_clicked()
{
    bool ok;
    QString exifcom = QInputDialog::getText(this, "EXIF Comment", "Комментарий:", QLineEdit::Normal, usrcommnt, &ok);
    if (ok && !exifcom.isEmpty()) {
        // Write
        cdr = QDir::currentPath();
        QString targetjpg = (cdr+"/DB/"+cutit+"/"+listjpg[jpgcstate]);
        QFile qfff(targetjpg);
        if(!qfff.exists()) return;
        std::string pthjpg = targetjpg.toStdString();
        std::string textexif = exifcom.toStdString();
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pthjpg);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        exifData["Exif.Photo.UserComment"] = "charset=\"Unicode\" "+textexif;
        image->writeMetadata();

        // Read
        cdr = QDir::currentPath();
        if(!qfff.exists()) return;
        image->readMetadata();
        Exiv2::Exifdatum& tag = exifData["Exif.Photo.UserComment"];
        std::string usrcomm = tag.toString();
        usrcomm.erase(0, 18);
        usrcommnt = QString::fromStdString(usrcomm);
        ui->exif2label->clear();
        ui->exif2label->setText(usrcommnt);
    }
}

// Menu Settings
void MainWindow::on_action_settings_menu_triggered()
{
    SettingsDialog setdialog;
    setdialog.setModal(true);
    setdialog.fontsetsize(fontsize);
    setdialog.exec();
    fontsize = setdialog.fntsz();

    if(fontsize != 0) {
        ui->plainTextEdit->setFont(QFont ("", fontsize));

        QSqlQuery query;
        query.prepare("UPDATE info SET name='fontsize', value=? WHERE name='fontsize'");
        query.bindValue(0, fontsize);
        query.exec();
    }
}

