#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include "controller/language.h"
#include "view/mainwindow.h"



/* For settings used:
 *  Organization: "jlus-privat"
 *  product: "RAAM-WP"
 *
 *  KEY                     DEFAULT         POSSIBLE
 *  language                "en"            "de", "en"
 *  keepintray              true            true, false
 *  autostart               "tray"          "tray", "full", "none"
 *  port                    1030            1024 - 65535
 *  startupupdatecheck      true            true, false*/
class MainController: public QObject {
    Q_OBJECT


public:
    MainController(QObject* = nullptr);


private:
    MainWindow *w = nullptr;
    QTranslator *translator = new QTranslator();
    QSettings *qSettings = nullptr;

    void updateLanguageController(QString);


private slots:
    void updateSettings();
    void resetSettings();


};

#endif // MAINCONTROLLER_H
