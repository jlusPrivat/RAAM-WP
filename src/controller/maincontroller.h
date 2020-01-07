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



/**
 * \brief The Controller keeping all
 *
 * Registry edits:
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
 *  startupupdatecheck      true            true, false
*/
class MainController: public QObject {
    Q_OBJECT
private:
    enum Settingskey {
        E_LANGUAGE, E_SERVERID, E_KEEPINTRAY, E_AUTOSTART, E_PORT,
        E_STARTUPUDATECHECK
    };


public:
    /**
     * Manages Settings, the view, the server, the clients, everything.
     * Therefore it has to be created when the application starts.
     *
     * 1. Loads the Settings as described in class
     * 2. Loads the selected language for display
     * 3. Creates MainWindow (and subwidgets) and
     * connects all signals to this controller
     * 4. Loads the clients into the gui
     * 5. Manages Tray Icon / Window visibility
     */
    MainController(QObject* = nullptr);


private:
    // properties
    MainWindow *w = nullptr;
    QTranslator *translator = new QTranslator(this);
    QSettings *qSettings = nullptr;
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QList<Client*> clients;

    // methos
    /// Selects in model currently selected language (does not visualize)
    void updateLanguageController(QString);
    /// Reads the settings with the given key. If not set, read default.
    QVariant readSetting(Settingskey);
    /// Writes the settings with the given values. Does not update view.
    void writeSetting(Settingskey, QVariant);
    /// Gets a pointer to a Client by its ID. Returns null, if not exist.
    Client* getClientById(QString);
    /// Determines the Icon for representing the clients state
    ClientView::IconType determineClientIcon(Client*);


private slots:
    // mainwindow
    /// if called with false: Minimize to tray, if setting says so;
    /// if called with true: Close application
    void parseCloseRequest(bool);
    /// open application from tray and hide tray icon
    void parseOpenRequest();

    // settings view
    /// connect to the server asking for the current
    /// version and display in statusbar
    void checkForUpdates();
    /// save all settings from settings tab
    void saveSettings();
    /// reset settings in settings tab to current values
    void resetSettings();

    // client view
    /// adds a new client to model and view
    void addNewClient();
    /// removes a client from model and view
    void removeClient(QString);
    /// selects a client in view
    void selectClient(QString);
    /// disables all client settings
    void unselectClient();
    /// saves client configuration as in the view
    void saveClient(QString);
    /// displays a window for the pairing information
    //void openPairWindow(QString);
    /// disconnects a client from this server
    void disconnectClient(QString);


};



#endif // MAINCONTROLLER_H
