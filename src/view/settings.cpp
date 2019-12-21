#include "settings.h"



Settings::Settings (QWidget *parent)
    : QScrollArea(parent) {
    // config the main widget
    setBackgroundRole(QPalette::Light);
    QFormLayout *layout = new QFormLayout(this);

    // keep in tray
    QVBoxLayout *lKeepInTray = new QVBoxLayout();
    lKeepInTray->addWidget(keepInTray);
    lKeepInTray->addWidget(new QLabel(tr("Keep this application running in "
                                         "tray, after it was closed"), this));
    layout->addRow(tr("Keep in tray:"), lKeepInTray);
}
