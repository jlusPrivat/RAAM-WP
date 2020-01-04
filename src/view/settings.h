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



class Settings: public QScrollArea {
    Q_OBJECT


public:
    Settings(QWidget*);
    QLineEdit *serverId = new QLineEdit(this);
    QCheckBox *keepInTray = new QCheckBox(this);
    QRadioButton *autostartFull = new QRadioButton(this);
    QRadioButton *autostartTray = new QRadioButton(this);
    QRadioButton *autostartNone = new QRadioButton(this);
    QSpinBox *port = new QSpinBox(this);
    QComboBox *language = new QComboBox(this);
    QCheckBox *startupUpdateCheck = new QCheckBox(this);


signals:
    void checkForUpdates();
    void closeRequested(bool);
    void settingsUpdated();
    void settingsReset();


private:
    QLabel *warningServerId = nullptr;


};

#endif // SETTINGS_H
