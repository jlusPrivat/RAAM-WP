#include "settings.h"



Settings::Settings (QWidget *parent)
    : QScrollArea(parent) {
    // config the main widget
    setBackgroundRole(QPalette::Light);
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *wForm = new QWidget(this);
    setWidget(wForm);
    QFormLayout *layout = new QFormLayout(wForm);
    layout->setSpacing(20);

    // keep in tray
    keepInTray->setText(tr("Keep this application running in "
                           "tray when closing it"));
    layout->addRow(tr("Keep in tray:"), keepInTray);

    // autostart
    QWidget *wAutostart = new QWidget(wForm);
    layout->addRow(tr("Autostart:"), wAutostart);
    QVBoxLayout *lAutostart = new QVBoxLayout(wAutostart);
    lAutostart->setMargin(0);
    autostartFull->setText(tr("Start RAAM, when logging in into windows"));
    lAutostart->addWidget(autostartFull);
    autostartTray->setText(tr("Start RAAM in tray, when logging in into windows"));
    lAutostart->addWidget(autostartTray);
    autostartNone->setText(tr("Do not start RAAM with windows"));
    lAutostart->addWidget(autostartNone);

    // port
    port->setRange(1025, 65535);
    port->setFixedWidth(300);
    layout->addRow(tr("Network port:"), port);

    // language
    language->addItem(QIcon(":/imgs/flag-de.png"), "Deutsch");
    language->addItem(QIcon(":/imgs/flag-uk.png"), "English");
    language->setFixedWidth(300);
    layout->addRow(tr("Language:"), language);

    // startup update check
    QWidget *wUpdateCheck = new QWidget(wForm);
    layout->addRow(tr("Update check:"), wUpdateCheck);
    QVBoxLayout *lUpdateCheck = new QVBoxLayout(wUpdateCheck);
    lUpdateCheck->setMargin(0);
    startupUpdateCheck->setText(tr("Check for updates, when the application starts"));
    lUpdateCheck->addWidget(startupUpdateCheck);
    QPushButton *buttonCheckUpdate = new QPushButton(
                tr("Check for updates now"), wUpdateCheck);
    buttonCheckUpdate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lUpdateCheck->addWidget(buttonCheckUpdate);

    // buttons at the end
    QHBoxLayout *lButtons = new QHBoxLayout;
    QPushButton *buttonSave = new QPushButton(tr("Save"), wForm);
    buttonSave->setStyleSheet("font-weight: bold;");
    lButtons->addWidget(buttonSave);
    QPushButton *buttonReset = new QPushButton(tr("Reset"), wForm);
    lButtons->addWidget(buttonReset);
    QPushButton *buttonClose = new QPushButton(tr("Close application"), wForm);
    buttonClose->setStyleSheet("color: red;");
    lButtons->addWidget(buttonClose);
    layout->addRow(lButtons);
}
