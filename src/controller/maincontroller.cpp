#include "maincontroller.h"



MainController::MainController (QObject *parent)
    : QObject(parent), lockFile(QDir::temp().absoluteFilePath("RAAM-WP.lock")) {
    // load the settings
    qSettings = new QSettings("jlus-privat", "RAAM-WP", this);

    // load the translation
    QString configuredLanguage = readSetting(E_LANGUAGE).toString();
    translator->load(QLocale(configuredLanguage),
                    "raam-wp", "_", ":/translations");
    qApp->installTranslator(translator);
    updateLanguageController(configuredLanguage);

    // check, if an instance is already running
    if (!lockFile.tryLock(100)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Application already running"));
        msgBox.setText(tr("This application is already running in another instance"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        exit(0);
        return;
    }

    // create the window
    w = new MainWindow();
    qApp->setQuitOnLastWindowClosed(false);
    resetSettings();

    // connect the signals with the main window
    connect(w, &MainWindow::sigCloseRequested,
            this, &MainController::parseCloseRequest);
    connect(w, &MainWindow::sigTrayOpenApp,
            this, &MainController::parseOpenRequest);
    connect(w, &MainWindow::sigTrayCloseApp,
            this, [&]{parseCloseRequest(true);});
    // connect the signals with the settings tab
    connect(w->settingsTab, &SettingsView::sigCheckForUpdates,
            this, &MainController::checkForUpdates);
    connect(w->settingsTab, &SettingsView::sigCloseRequested,
            this, &MainController::parseCloseRequest);
    connect(w->settingsTab, &SettingsView::sigSettingsUpdated,
            this, &MainController::saveSettings);
    connect(w->settingsTab, &SettingsView::sigSettingsReset,
            this, &MainController::resetSettings);
    // connect the signals with the clients tab
    connect(w->clientTab, &ClientView::sigAddNewClient,
            this, &MainController::addNewClient);
    connect(w->clientTab, &ClientView::sigRemoveClient,
            this, &MainController::removeClient);
    connect(w->clientTab, &ClientView::sigSelectClient,
            this, &MainController::selectClient);
    connect(w->clientTab, &ClientView::sigUnselectClient,
            this, &MainController::unselectClient);
    connect(w->clientTab, &ClientView::sigSaveClient,
            this, &MainController::saveClient);
    connect(w->clientTab, &ClientView::sigOpenPairWindow,
            this, &MainController::openPairWindow);
    connect(w->clientTab, &ClientView::sigDisconnectClient,
            this, &MainController::disconnectClient);

    // load all clients into the listview
    qSettings->beginGroup("clients");
    QStringList allClientIds = qSettings->childGroups();
    qSettings->endGroup();
    int numClients = allClientIds.count();
    for (int i = 0; i < numClients; i++) {
        QString clientName = allClientIds.at(i);
        Client *newClient = new Client(clientName, qSettings, this);
        newClient->loadConfig();
        clients.append(newClient);
        w->clientTab->lstAddItem(clientName, newClient->getConnectionState());
        connect(newClient, &Client::sigCommanded,
                this, &MainController::clientCommanded);
        connect(newClient, &Client::sigPairedChanged,
                this, &MainController::clientPairedChanged);
    }

    // show the tray icon
    if (qApp->arguments().contains("tray"))
        w->trayIcon->show();
    // show the window
    else
        w->show();

    // check for updates
    if (readSetting(E_STARTUPUDATECHECK).toBool())
        checkForUpdates();

    // start the model
    audioController = new AudioController(this);
    connect(audioController, &AudioController::broadcastCommand,
            this, &MainController::broadcastCommand);

    // start the tcp server (if enabled)
    startTCPServer();
}



void MainController::updateLanguageController (QString langShort) {
    const int numLangs = Language::languages.count();
    for (int i = 0; i < numLangs; i++) {
        Language *lang = Language::languages.at(i);
        lang->isSelected = (langShort == lang->nameShort);
    }
}



QVariant MainController::readSetting (Settingskey key) {
    switch (key) {
    case E_LANGUAGE:
        return qSettings->value("language", "en");
    case E_SERVERID:
        return qSettings->value("serverid", "RAAM SERVER");
    case E_KEEPINTRAY:
        return qSettings->value("keepintray", true);
    case E_AUTOSTART:
        return qSettings->value("autostart", "tray");
    case E_PORT:
        return qSettings->value("port", 1030);
    case E_STARTUPUDATECHECK:
        return qSettings->value("startupupdatecheck", true);
    case E_TCP_ENABLED:
        return qSettings->value("tcpenabled", true);
    case E_BLE_ENABLED:
        return qSettings->value("bleenabled", true);
    }
    return 0;
}



void MainController::writeSetting (Settingskey key, QVariant content) {
    switch (key) {
    case E_LANGUAGE:
        qSettings->setValue("language", content);
        break;
    case E_SERVERID:
        qSettings->setValue("serverid", content);
        break;
    case E_KEEPINTRAY:
        qSettings->setValue("keepintray", content);
        break;
    case E_AUTOSTART:
        qSettings->setValue("autostart", content);
        break;
    case E_PORT:
        qSettings->setValue("port", content);
        break;
    case E_STARTUPUDATECHECK:
        qSettings->setValue("startupupdatecheck", content);
        break;
    case E_TCP_ENABLED:
        qSettings->setValue("tcpenabled", content);
        break;
    case E_BLE_ENABLED:
        qSettings->setValue("bleenabled", content);
        break;
    }
}



Client* MainController::getClientById (QString id) {
    int count = clients.count();
    for (int i = 0; i < count; i++) {
        Client* ce = clients.at(i);
        if (ce->getId() == id)
            return ce;
    }
    return nullptr;
}



void MainController::startTCPServer () {
    if (tcpServer) {
        // unpair all
        int countClients = clients.size();
        for (int i = 0; i < countClients; i++)
            clients.at(i)->unpair();
        // close previous connection
        tcpServer->close();
        disconnect(tcpServer, &QTcpServer::newConnection,
                   this, &MainController::acceptConnection);
        delete(tcpServer);
    }

    // create new tcp server
    if (readSetting(E_TCP_ENABLED).toBool()) {
        tcpServer = new QTcpServer(this);
        connect(tcpServer, &QTcpServer::newConnection,
                this, &MainController::acceptConnection);
        if (!tcpServer->listen(QHostAddress::Any, readSetting(E_PORT).toInt())) {
            // !!! error handling
        }
    }
}



void MainController::broadcastCommand (Command &command) {
    int count = clients.count();
    for (int i = 0; i < count; i++)
        clients.at(i)->sendCommand(command);
}



void MainController::parseCloseRequest (bool force) {
    if (force || !readSetting(E_KEEPINTRAY).toBool())
        // fully close application
        qApp->quit();
    else {
        // just minimize to tray
        w->trayIcon->show();
        w->hide();
    }
}



void MainController::parseOpenRequest () {
    if (!w->isVisible()) {
        w->show();
        w->trayIcon->hide();
    }
}



void MainController::acceptConnection () {
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    if (!socket)
        return;

    // wait for 10 seconds or until the initialization finished
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]{
        // when the timer times out: close socket and delete
        socket->disconnectFromHost();
        socket->close();
        delete(socket);
        timer->stop();
        timer->deleteLater();
    });
    connect(socket, &QTcpSocket::readyRead, this, [=]{
        timer->stop();
        delete(timer);
        socket->disconnect();

        // forward the server to the client
        QString message = socket->readLine();
        QHash<QString, QString> *map = Client::parseMessage(message);
        QString clientId;
        if (!map || (clientId = map->value("c", "")).isEmpty()
                || map->value("a", "") != "init") {
            socket->close();
            return;
        }

        // get the hmac
        QByteArray hmac;
        if (socket->bytesAvailable() >= 32)
            hmac = socket->read(32);

        Client *client = getClientById(clientId);
        // no client found
        if (!client) {
            socket->write("CLIENT_UNKNOWN");
            socket->close();
        }
        // client not active or user rejected
        else if (!client->isActive() ||
                 (client->askBeforeConnect && !w->askForConnect(clientId))) {
            socket->write("CLIENT_DISABLED");
            socket->close();
        }
        // pairing is allowed
        else
            client->pair(socket, message, hmac);
    });
    timer->setSingleShot(true);
    timer->start(10000);
}



void MainController::clientPairedChanged (Client* client) {
    if (!client)
        return;
    QString id = client->getId();
    bool paired = client->isPaired();

    w->showMessage((paired ? tr("\"%1\" paired").arg(id)
                           : tr("\"%1\" unpaired").arg(id)),
                   client->showNotificationOnConnect);

    // update buttons and icons
    if (w->clientTab->currentItem() == id) {
        w->clientTab->confBtnPair->setDisabled(paired);
        w->clientTab->confBtnDisconnect->setDisabled(!paired);
    }
    w->clientTab->updateItem(id, id, client->getConnectionState());
}



void MainController::clientCommanded (Client *client, Command &command) {
    // send initialization answer
    if (command.getAction() == "init") {
        Command c("init", Command::E_OUTBOUND);
        c.put("s", readSetting(E_SERVERID).toString());
        client->sendCommand(c);
    }

    // returns the info for the user
    else if (command.getAction() == "info") {
        Command c("inforeturn", Command::E_OUTBOUND);
        c.put("s", readSetting(E_SERVERID).toString());
        c.put("c", client->getId());
        c.put("sw", SOFTWARE_NAME);
        c.put("v", VERSION);
        c.put("sv", SPECS_VERSION);
        client->sendCommand(c);
    }

    // maybe the audio controller can work with it
    else
        audioController->clientCommanded(client, command);
}



void MainController::checkForUpdates () {
    QObject::connect(networkManager, &QNetworkAccessManager::finished,
                     this, [&](QNetworkReply* reply) {
        QNetworkReply::NetworkError error = reply->error();
        QString latestVersion = reply->readAll();
        reply->deleteLater();

        // check for error
        if (error != QNetworkReply::NoError) {
            w->showMessage(tr("There was a network error."), false);
            return;
        }

        // compare the output
        QStringList latestVersionParts = latestVersion.split(" ").at(0).split(".");
        QStringList currentVersionParts = QString(VERSION)
                .split(" ").at(0).split(".");
        int numLatestParts = latestVersionParts.count();
        int numCurrentParts = currentVersionParts.count();
        bool isNewer = false;
        for (int i = 0; i < numLatestParts; i++) {
            if (i >= numCurrentParts) {
                isNewer = true;
                break;
            }
            else if (latestVersionParts.at(i).toInt()
                     > currentVersionParts.at(i).toInt()) {
                isNewer = true;
                break;
            }
        }

        // if it is newer, output a message
        if (isNewer)
            w->showMessage(tr("There is a newer version available!"), true);
        else
            w->showMessage(tr("This is the most recent version!"), false);
    });
    networkManager->get(QNetworkRequest(QUrl(VERSION_DESCRIPTION_URL)));
}



void MainController::saveSettings () {
    SettingsView *s = w->settingsTab;

    // inform clients about new server id
    if (s->serverId->hasAcceptableInput()
            && readSetting(E_SERVERID).toString() != s->serverId->text()) {
        writeSetting(E_SERVERID, s->serverId->text());
        Command c("changeServerId", Command::E_OUTBOUND);
        c.put("s", s->serverId->text());
        broadcastCommand(c);
    }
    else
        s->serverId->setText(readSetting(E_SERVERID).toString());

    // keep in tray
    writeSetting(E_KEEPINTRAY, s->keepInTray->isChecked());

    // autostart
    QSettings bootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows"
                             "\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString appPath = qApp->applicationFilePath().replace('/', '\\');
    QString autostart;
    if (s->autostartFull->isChecked()) {
        autostart = "full";
        bootUpSettings.setValue("raam", appPath);
    }
    else if (s->autostartNone->isChecked()) {
        autostart = "none";
        if (bootUpSettings.contains("raam"))
            bootUpSettings.remove("raam");
    }
    else {
        autostart = "tray";
        bootUpSettings.setValue("raam", appPath + " tray");
    }
    writeSetting(E_AUTOSTART, autostart);

    // automatic update check
    writeSetting(E_STARTUPUDATECHECK, s->startupUpdateCheck->isChecked());

    // port and TCP enabled
    bool newTcpEnabled = s->tcpEnabled->isChecked();
    int newPort = s->port->value();
    if (newPort != readSetting(E_PORT).toInt()
            || newTcpEnabled != readSetting(E_TCP_ENABLED).toBool()) {
        writeSetting(E_PORT, newPort);
        writeSetting(E_TCP_ENABLED, newTcpEnabled);
        startTCPServer();
    }

    // BLE enabled
    writeSetting(E_BLE_ENABLED, s->bleEnabled->isChecked());

    // language
    QString newLanguage = s->language->currentData().toString();
    if (newLanguage != readSetting(E_LANGUAGE)) {
        writeSetting(E_LANGUAGE, newLanguage);
        updateLanguageController(newLanguage);
        if (w->askForRestart()) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }

    s->buttonSave->setDisabled(true);
}



void MainController::resetSettings () {
    SettingsView *s = w->settingsTab;

    // server id
    s->serverId->setText(readSetting(E_SERVERID).toString());

    // keep in tray
    s->keepInTray->setChecked(readSetting(E_KEEPINTRAY).toBool());

    // autostart
    QString autostart = readSetting(E_AUTOSTART).toString();
    if (autostart == "full")
        s->autostartFull->setChecked(true);
    else if (autostart == "none")
        s->autostartNone->setChecked(true);
    else
        s->autostartTray->setChecked(true);

    // port
    s->port->setValue(readSetting(E_PORT).toInt());

    // automatic update check
    s->startupUpdateCheck->setChecked(readSetting(E_STARTUPUDATECHECK).toBool());

    // TCP enabled
    s->tcpEnabled->setChecked(readSetting(E_TCP_ENABLED).toBool());

    // BLE enabled
    s->bleEnabled->setChecked(readSetting(E_BLE_ENABLED).toBool());

    // language
    auto langs = Language::languages;
    const int numEntries = langs.count();
    for (int i = 0; i < numEntries; i++) {
        Language *lang = langs.at(i);
        if (lang->isSelected)
            s->language->setCurrentIndex(i);
    }

    s->buttonSave->setDisabled(true);
}



void MainController::addNewClient () {
    // find a good new name
    //: Translation must obey the rules for client ids
    QString newNameBase = tr("new client");
    int newNameNum = 1;
    QString newName = newNameBase;
    while (getClientById(newName))
        newName = QStringLiteral("%1 %2").arg(newNameBase).arg(newNameNum++);
    w->clientTab->lstAddItem(newName, Client::E_INACTIVE);

    // generate a new client
    Client *newClient = new Client(newName, qSettings, this);
    newClient->setActive(false);
    newClient->generateSecretKey();
    newClient->saveConfig();
    clients.append(newClient);

    // add the listeners for the newly created client
    connect(newClient, &Client::sigCommanded,
            this, &MainController::clientCommanded);
    connect(newClient, &Client::sigPairedChanged,
            this, &MainController::clientPairedChanged);
}



void MainController::removeClient (QString id) {
    Client *client = getClientById(id);
    if (!client)
        return;

    clients.removeAll(client);
    client->disconnect();
    client->removeFromConfig();
    delete client;
    w->clientTab->lstRemoveItem(id);
}



void MainController::selectClient (QString id) {
    Client *client = getClientById(id);
    if (!client) {
        unselectClient();
        return;
    }
    ClientView *cw = w->clientTab;

    // set fields content
    cw->confClientId->setText(client->getId());
    cw->confActive->setChecked(client->isActive());
    cw->confDescription->setPlainText(client->description);
    cw->confAskPairing->setChecked(client->askBeforeConnect);
    cw->confShowNotification->setChecked(client->showNotificationOnConnect);
    cw->confDebugMode->setChecked(client->debugmode);
    cw->confOnlyPluggedIn->setChecked(client->isOnlyPluggedInDevices());

    // set fields enabled
    cw->listBtnRemove->setDisabled(false);
    cw->wConfDiag->setDisabled(false);
    cw->confBtnSave->setDisabled(true);
    cw->confBtnPair->setDisabled(client->isPaired());
    cw->confBtnDisconnect->setEnabled(client->isPaired());
}



void MainController::unselectClient () {
    ClientView *cw = w->clientTab;

    // clear fields
    cw->confClientId->clear();
    cw->confActive->setChecked(false);
    cw->confDescription->clear();
    cw->confAskPairing->setChecked(false);
    cw->confShowNotification->setChecked(false);
    cw->confOnlyPluggedIn->setChecked(false);

    // set fields disabled
    cw->listBtnRemove->setDisabled(true);
    cw->wConfDiag->setDisabled(true);
}



void MainController::saveClient (QString id) {
    Client *client = getClientById(id);
    if (!client) {
        unselectClient();
        return;
    }
    ClientView *cw = w->clientTab;

    // set the properties
    if (cw->confClientId->hasAcceptableInput())
        client->setId(cw->confClientId->text());
    else
        cw->confClientId->setText(client->getId());
    client->setActive(cw->confActive->isChecked());
    client->description = cw->confDescription->toPlainText();
    client->askBeforeConnect = cw->confAskPairing->isChecked();
    client->showNotificationOnConnect = cw->confShowNotification->isChecked();
    client->setOnlyPluggedInDevices(cw->confOnlyPluggedIn->isChecked());
    client->debugmode = cw->confDebugMode->isChecked();
    client->saveConfig();

    // update the listview, if needed
    cw->updateItem(id, client->getId(), client->getConnectionState());

    // disable the save field
    cw->confBtnSave->setDisabled(true);
}



void MainController::openPairWindow (QString id) {
    Client *client = getClientById(id);
    if (!client) {
        unselectClient();
        return;
    }

    PairingWindow *pw = new PairingWindow(w);

    // set server id
    pw->wServerId->setText(readSetting(E_SERVERID).toString());

    // set server ip
    QString serverIp = tr("Not connected");
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
              serverIp = address.toString();
    }
    pw->wServerIp->setText(serverIp);

    // set server port
    pw->wServerPort->setText(readSetting(E_PORT).toString());

    // set client secret
    pw->wClientSecret->setText(client->getSecretKey().toBase64());

    // set client id
    pw->setClientId(id);

    // display
    pw->generateQr("!!!QRTEST");
    pw->show();
}



void MainController::disconnectClient (QString id) {
    Client *client = getClientById(id);
    if (!client) {
        unselectClient();
        return;
    }

    // unpair
    client->unpair();
}
