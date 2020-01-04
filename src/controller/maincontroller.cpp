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

    // connect the signals with the window
    QObject::connect(w, &MainWindow::closeRequested,
                     this, &MainController::parseCloseRequest);
    QObject::connect(w, &MainWindow::trayOpenApp,
                     this, &MainController::parseOpenRequest);
    QObject::connect(w, &MainWindow::trayCloseApp,
                     this, [&]{parseCloseRequest(true);});
    QObject::connect(w->settingsTab, &Settings::checkForUpdates,
                     this, &MainController::checkForUpdates);
    QObject::connect(w->settingsTab, &Settings::closeRequested,
                     this, &MainController::parseCloseRequest);
    QObject::connect(w->settingsTab, &Settings::settingsUpdated,
                     this, &MainController::updateSettings);
    QObject::connect(w->settingsTab, &Settings::settingsReset,
                     this, &MainController::resetSettings);

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
    Settings *s = w->settingsTab;

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
    Settings *s = w->settingsTab;

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
