#include "settingsview.h"



SettingsView::SettingsView (QWidget *parent)
    : QWidget(parent) {
    // config the main widget
    setBackgroundRole(QPalette::Light);
    QVBoxLayout *layout = new QVBoxLayout(this);

    // config the scrollable settings
    QScrollArea *wScrollable = new QScrollArea(this);
    layout->addWidget(wScrollable);
    wScrollable->setFrameShape(QFrame::NoFrame);
    wScrollable->setWidgetResizable(true);
    wScrollable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    wScrollable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // another form has to be encapsulated
    QWidget *wForm = new QWidget(wScrollable);
    wScrollable->setWidget(wForm);
    QFormLayout *lForm = new QFormLayout(wForm);
    lForm->setSpacing(15);

    // server id
    QWidget *wServerId = new QWidget(wForm);
    lForm->addRow(tr("Server-ID:"), wServerId);
    QHBoxLayout *lServerId = new QHBoxLayout(wServerId);
    lServerId->setAlignment(Qt::AlignLeft);
    lServerId->setMargin(0);
    // server Id field
    serverId = new QLineEdit(wServerId);
    QRegExp rx("[a-zA-Z][a-zA-Z0-9 _\\-]{1,23}[a-zA-Z0-9]");
    serverId->setValidator(new QRegExpValidator(rx, this));
    serverId->setFixedWidth(300);
    lServerId->addWidget(serverId);
    // server id warning
    QPixmap pmWarningIcon(":/imgs/warning.png");
    warningServerId = new QLabel(wServerId);
    warningServerId->setPixmap(pmWarningIcon);
    warningServerId->setToolTip(tr("- 3 to 100 characters allowed\n"
                                   "- Only alphanumeric, space and underscore\n"
                                   "- Beginning only letters"));
    warningServerId->setVisible(false);
    lServerId->addWidget(warningServerId);
    connect(serverId, &QLineEdit::textChanged, this, [&]{
        warningServerId->setVisible(!serverId->hasAcceptableInput());
    });

    // keep in tray
    keepInTray = new QCheckBox(wForm);
    keepInTray->setText(tr("Keep this application running in "
                           "tray when closing it"));
    lForm->addRow(tr("Keep in tray:"), keepInTray);

    // autostart
    QWidget *wAutostart = new QWidget(wForm);
    lForm->addRow(tr("Autostart:"), wAutostart);
    QVBoxLayout *lAutostart = new QVBoxLayout(wAutostart);
    lAutostart->setMargin(0);
    autostartFull = new QRadioButton(wAutostart);
    autostartFull->setText(tr("Start RAAM, when logging in into windows"));
    lAutostart->addWidget(autostartFull);
    autostartTray = new QRadioButton(wAutostart);
    autostartTray->setText(tr("Start RAAM in tray, when logging in into windows"));
    lAutostart->addWidget(autostartTray);
    autostartNone = new QRadioButton(wAutostart);
    autostartNone->setText(tr("Do not start RAAM with windows"));
    lAutostart->addWidget(autostartNone);

    // port
    port = new QSpinBox(wForm);
    port->setRange(1024, 65535);
    port->setFixedWidth(300);
    lForm->addRow(tr("Network port:"), port);

    // language
    language = new QComboBox(wForm);
    auto langs = Language::languages;
    const int numEntries = langs.count();
    for (int i = 0; i < numEntries; i++) {
        Language *lang = langs.at(i);
        language->addItem(QIcon(":/imgs/flag-" + lang->nameShort + ".png"),
                          lang->nameFull, lang->nameShort);
    }
    language->setFixedWidth(300);
    lForm->addRow(tr("Language:"), language);

    // startup update check
    startupUpdateCheck = new QCheckBox(wForm);
    QWidget *wUpdateCheck = new QWidget(wForm);
    lForm->addRow(tr("Update check:"), wUpdateCheck);
    QVBoxLayout *lUpdateCheck = new QVBoxLayout(wUpdateCheck);
    lUpdateCheck->setMargin(0);
    startupUpdateCheck->setText(tr("Check for updates, when the application starts"));
    lUpdateCheck->addWidget(startupUpdateCheck);
    QPushButton *buttonCheckUpdate = new QPushButton(
                tr("Check for updates now"), wUpdateCheck);
    connect(buttonCheckUpdate, &QPushButton::clicked,
            this, &SettingsView::sigCheckForUpdates);
    buttonCheckUpdate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lUpdateCheck->addWidget(buttonCheckUpdate);


    // buttons at the end
    QWidget *wButtons = new QWidget(this);
    layout->addWidget(wButtons);
    QHBoxLayout *lButtons = new QHBoxLayout(wButtons);
    lButtons->setMargin(0);

    // save button incl. all "enable" connections
    buttonSave = new QPushButton(tr("Save"), wForm);
    connect(buttonSave, &QPushButton::clicked,
            this, &SettingsView::sigSettingsUpdated);
    connect(serverId, &QLineEdit::textChanged,
            this, [&]{buttonSave->setDisabled(false);});
    connect(keepInTray, &QCheckBox::stateChanged,
            this, [&]{buttonSave->setDisabled(false);});
    connect(autostartFull, &QRadioButton::toggled,
            this, [&]{buttonSave->setDisabled(false);});
    connect(autostartTray, &QRadioButton::toggled,
            this, [&]{buttonSave->setDisabled(false);});
    connect(autostartNone, &QRadioButton::toggled,
            this, [&]{buttonSave->setDisabled(false);});
    connect(port, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [&]{buttonSave->setDisabled(false);});
    connect(language, &QComboBox::currentTextChanged,
            this, [&]{buttonSave->setDisabled(false);});
    connect(startupUpdateCheck, &QCheckBox::stateChanged,
            this, [&]{buttonSave->setDisabled(false);});
    buttonSave->setDefault(true);
    lButtons->addWidget(buttonSave);

    // reset button
    QPushButton *buttonReset = new QPushButton(tr("Reset"), wForm);
    connect(buttonReset, &QPushButton::clicked,
            this, &SettingsView::sigSettingsReset);
    buttonReset->setToolTip(tr("Reset form to abort editing configuration"));
    lButtons->addWidget(buttonReset);

    // close button
    QPushButton *buttonClose = new QPushButton(tr("Close application"), wForm);
    connect(buttonClose, &QPushButton::clicked,
            this, [&]{sigCloseRequested(true);});
    buttonClose->setToolTip(tr("Fully close the application (no tray)"));
    buttonClose->setStyleSheet("color: red;");
    lButtons->addWidget(buttonClose);
}
