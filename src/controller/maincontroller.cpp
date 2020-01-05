#include "maincontroller.h"



MainController::MainController (QObject *parent)
    : QObject(parent) {
    // load the settings
    qSettings = new QSettings("jlus-privat", "RAAM-WP", this);

    // load the translation
    QString configuredLanguage = readSetting(E_LANGUAGE).toString();
    translator->load(QLocale(configuredLanguage),
                    "raam-wp", "_", ":/translations");
    qApp->installTranslator(translator);
    updateLanguageController(configuredLanguage);

    // create the window
    w = new MainWindow();
    resetSettings();

    // connect the signals with the main window
    connect(w, &MainWindow::closeRequested,
            this, &MainController::parseCloseRequest);
    connect(w, &MainWindow::trayOpenApp,
            this, &MainController::parseOpenRequest);
    connect(w, &MainWindow::trayCloseApp,
            this, [&]{parseCloseRequest(true);});
    // connect the signals with the settings tab
    connect(w->settingsTab, &SettingsView::checkForUpdates,
            this, &MainController::checkForUpdates);
    connect(w->settingsTab, &SettingsView::closeRequested,
            this, &MainController::parseCloseRequest);
    connect(w->settingsTab, &SettingsView::settingsUpdated,
            this, &MainController::updateSettings);
    connect(w->settingsTab, &SettingsView::settingsReset,
            this, &MainController::resetSettings);
    // connect the signals with the clients tab
    connect(w->clientTab, &ClientView::addNewClient,
            this, &MainController::addNewClient);
    connect(w->clientTab, &ClientView::removeClient,
            this, &MainController::removeClient);
    connect(w->clientTab, &ClientView::selectClient,
            this, &MainController::selectClient);
    connect(w->clientTab, &ClientView::unselectClient,
            this, &MainController::unselectClient);
    connect(w->clientTab, &ClientView::saveClient,
            this, &MainController::saveClient);
    connect(w->clientTab, &ClientView::disconnectClient,
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
        w->clientTab->lstAddItem(clientName, determineClientIcon(newClient));
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



ClientView::IconType MainController::determineClientIcon (Client* client) {
    if (client->isPaired())
        return ClientView::E_ICON_PAIRED;
    else if (client->isActive())
        return ClientView::E_ICON_ACTIVE;
    return ClientView::E_ICON_INACTIVE;
}



void MainController::parseCloseRequest (bool force) {
    if (force || !readSetting(E_KEEPINTRAY).toBool()) {
        // fully close application
        qApp->quit();
    }
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



void MainController::updateSettings () {
    SettingsView *s = w->settingsTab;

    // server id
    if (s->serverId->hasAcceptableInput())
        writeSetting(E_SERVERID, s->serverId->text());
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

    // port
    writeSetting(E_PORT, s->port->value());

    // automatic update check
    writeSetting(E_STARTUPUDATECHECK, s->startupUpdateCheck->isChecked());

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
    w->clientTab->lstAddItem(newName, ClientView::E_ICON_INACTIVE);

    // generate a new client
    Client *newClient = new Client(newName, qSettings, this);
    newClient->setActive(false);
    newClient->saveConfig();
    clients.append(newClient);
}



void MainController::removeClient (QString id) {
    Client *client = getClientById(id);
    if (!client)
        return;

    clients.removeAll(client);
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
    cw->confOnlyPluggedIn->setChecked(client->isOnlyPluggedInDevices());

    // set fields enabled
    cw->listBtnRemove->setDisabled(false);
    cw->confBox->setDisabled(false);
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
    cw->confBox->setDisabled(true);
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
    client->saveConfig();

    // update the listview, if needed
    cw->updateItem(id, client->getId(), determineClientIcon(client));

    // disable the save field
    cw->confBtnSave->setDisabled(true);
}



void MainController::disconnectClient (QString id) {
    Client *client = getClientById(id);
    if (!client) {
        unselectClient();
        return;
    }
    ClientView *cw = w->clientTab;

    // unpair
    client->unpair();
    // update buttons
    cw->confBtnPair->setDisabled(false);
    cw->confBtnDisconnect->setDisabled(true);
    // update icon
    cw->updateItem(id, id, determineClientIcon(client));
}
