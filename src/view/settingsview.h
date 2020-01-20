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



/**
 * \brief SettingsView presents the configuration to the user
*/
class SettingsView: public QWidget {
    Q_OBJECT


public:
    /**
     * \brief Constructor
     *
     * 1. Generating the view
     * 2. Adding the validator to the Server ID Field
     *
     * Does not provision any information into the fields. This has
     * to be done by the controller.
     * Does not register itself as a subwidget of passes widget.
     * The Save button enable state must be controlled by controller.
     */
    SettingsView(QWidget*);

    QLineEdit *serverId = nullptr;
    QCheckBox *keepInTray = nullptr;
    QRadioButton *autostartFull = nullptr;
    QRadioButton *autostartTray = nullptr;
    QRadioButton *autostartNone = nullptr;
    QSpinBox *port = nullptr;
    QComboBox *language = nullptr;
    QCheckBox *startupUpdateCheck = nullptr;
    QCheckBox *tcpEnabled = nullptr;
    QCheckBox *bleEnabled = nullptr;
    QCheckBox *debugMode = nullptr;
    QPushButton *buttonSave = nullptr;


signals:
    /// User has clicked on the check for updates button
    void sigCheckForUpdates();
    /// User has clicked on force close application button
    void sigCloseRequested(bool);
    /// User has requested to save the settings
    void sigSettingsUpdated();
    /// User has requested to reload the settings into the fields
    void sigSettingsReset();


private:
    void keyPressEvent(QKeyEvent *event) override;
    QLabel *warningServerId = nullptr;


};

#endif // SETTINGS_H
