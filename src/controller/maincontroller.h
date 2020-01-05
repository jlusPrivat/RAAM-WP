#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include <QtNetwork>
#include "controller/language.h"
#include "model/client.h"
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
private:
    enum Settingskey {
        E_LANGUAGE, E_SERVERID, E_KEEPINTRAY, E_AUTOSTART, E_PORT,
        E_STARTUPUDATECHECK
    };


public:
    MainController(QObject* = nullptr);


private:
    // properties
    MainWindow *w = nullptr;
    QTranslator *translator = new QTranslator(this);
    QSettings *qSettings = nullptr;
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QList<Client*> clients;

    // methos
    void updateLanguageController(QString);
    QVariant readSetting(Settingskey);
    void writeSetting(Settingskey, QVariant);
    Client* getClientById(QString);
    ClientView::IconType determineClientIcon(Client*);


private slots:
    // mainwindow
    void parseCloseRequest(bool);
    void parseOpenRequest();
    // settings view
    void checkForUpdates();
    void saveSettings();
    void resetSettings();
    // client view
    void addNewClient();
    void removeClient(QString);
    void selectClient(QString);
    void unselectClient();
    void saveClient(QString);
    //void openPairWindow(QString);
    void disconnectClient(QString);


};



#endif // MAINCONTROLLER_H
