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

    // connect the signals with the window
    QObject::connect(w->settingsTab, &Settings::settingsUpdated,
                     this, &MainController::updateSettings);

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
    QString newLanguage = w->settingsTab->language->currentData().toString();
    if (newLanguage != qSettings->value("language", "en")) {
        qSettings->setValue("language", newLanguage);
        updateLanguageController(newLanguage);
        if (w->askForRestart()) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}
