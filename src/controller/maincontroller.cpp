#include "maincontroller.h"



MainController::MainController (QObject *parent)
    : QObject(parent) {
    // load the settings
    qSettings = new QSettings("jlus-privat", "RAAM-WP", this);

    // load the translation
    QString configuredLanguage = qSettings->value("language", "en").toString();
    translator->load(QLocale(configuredLanguage),
                    "raam-wp", "_", ":/translations");
    qApp->installTranslator(translator);
    updateLanguageController(configuredLanguage);

    // create the window
    w = new MainWindow();
    resetSettings();

    // connect the signals with the window
    QObject::connect(w->settingsTab, &Settings::settingsUpdated,
                     this, &MainController::updateSettings);
    QObject::connect(w->settingsTab, &Settings::settingsReset,
                     this, &MainController::resetSettings);

    // show the window
    w->show();
}



void MainController::updateLanguageController (QString langShort) {
    const int numLangs = Language::languages.count();
    for (int i = 0; i < numLangs; i++) {
        Language *lang = Language::languages.at(i);
        lang->isSelected = (langShort == lang->nameShort);
    }
}



void MainController::updateSettings () {
    Settings *s = w->settingsTab;

    // keep in tray
    qSettings->setValue("keepintray", s->keepInTray->isChecked());

    // autostart
    QString autostart;
    if (s->autostartFull->isChecked())
        autostart = "full";
    else if (s->autostartNone->isChecked())
        autostart = "none";
    else
        autostart = "tray";
    qSettings->setValue("autostart", autostart);

    // port
    qSettings->setValue("port", s->port->value());

    // automatic update check
    qSettings->setValue("startupupdatecheck", s->startupUpdateCheck->isChecked());

    // language
    QString newLanguage = s->language->currentData().toString();
    if (newLanguage != qSettings->value("language", "en")) {
        qSettings->setValue("language", newLanguage);
        updateLanguageController(newLanguage);
        if (w->askForRestart()) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}



void MainController::resetSettings () {
    Settings *s = w->settingsTab;

    // keep in tray
    s->keepInTray->setChecked(qSettings->value("keepintray",
                                               true).toBool());

    // autostart
    QString autostart = qSettings->value("autostart", "tray").toString();
    if (autostart == "full")
        s->autostartFull->setChecked(true);
    else if (autostart == "none")
        s->autostartNone->setChecked(true);
    else
        s->autostartTray->setChecked(true);

    // port
    s->port->setValue(qSettings->value("port", 1030).toInt());

    // automatic update check
    s->startupUpdateCheck->setChecked(qSettings->value("startupupdatecheck",
                                                       true).toBool());

    // language
    auto langs = Language::languages;
    const int numEntries = langs.count();
    for (int i = 0; i < numEntries; i++) {
        Language *lang = langs.at(i);
        if (lang->isSelected)
            s->language->setCurrentIndex(i);
    }
}
