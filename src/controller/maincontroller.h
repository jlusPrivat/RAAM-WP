#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include <QtNetwork>
#include "controller/language.h"
#include "view/mainwindow.h"



/* Registry edits:
 *  HKEY_CURRENT_USER\Software\Microsoft
 *      \Windows\CurrentVersion\Run\raam
 *
 * For settings used:
 *  Organization: "jlus-privat"
 *  product: "RAAM-WP"
 *
 *  KEY                     DEFAULT         POSSIBLE
 *  language                "en"            "de", "en"
 *  serverid                "RAAM SERVER"   [a-zA-Z][a-zA-Z0-9 _]{2,100})
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
    QTranslator *translator = new QTranslator(this);
    QSettings *qSettings = nullptr;
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    void updateLanguageController(QString);


private slots:
    void parseCloseRequest(bool);
    void parseOpenRequest();
    void checkForUpdates();
    void updateSettings();
    void resetSettings();


};

#endif // MAINCONTROLLER_H
