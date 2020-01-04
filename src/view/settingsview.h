#ifndef SETTINGS_H
#define SETTINGS_H

#include <QScrollArea>
#include <QFormLayout>
#include <QLabel>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "controller/language.h"



class SettingsView: public QWidget {
    Q_OBJECT


public:
    SettingsView(QWidget*);
    QLineEdit *serverId = nullptr;
    QCheckBox *keepInTray = nullptr;
    QRadioButton *autostartFull = nullptr;
    QRadioButton *autostartTray = nullptr;
    QRadioButton *autostartNone = nullptr;
    QSpinBox *port = nullptr;
    QComboBox *language = nullptr;
    QCheckBox *startupUpdateCheck = nullptr;
    QPushButton *buttonSave = nullptr;


signals:
    void checkForUpdates();
    void closeRequested(bool);
    void settingsUpdated();
    void settingsReset();


private:
    QLabel *warningServerId = nullptr;


};

#endif // SETTINGS_H
