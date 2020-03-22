#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QObject>
#include <QScrollArea>
#include <QGroupBox>
#include <QListWidget>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include "model/client.h"



/**
 * \brief The ClientView class
 *
 * Displaying all clients and its configurations
 */
class ClientView: public QWidget {
    Q_OBJECT


public:
    // properties
    QPushButton *listBtnAdd = nullptr;
    QPushButton *listBtnRemove = nullptr;

    QGroupBox *wConfDiag = nullptr;
    QLineEdit *confClientId = nullptr;
    QCheckBox *confActive = nullptr;
    QPlainTextEdit *confDescription = nullptr;
    QCheckBox *confAskPairing = nullptr;
    QCheckBox *confShowNotification = nullptr;
    QCheckBox *confOnlyPluggedIn = nullptr;
    QCheckBox *confDebugMode = nullptr;
    QPushButton *confBtnSave = nullptr;
    QPushButton *confBtnReset = nullptr;
    QPushButton *confBtnPair = nullptr;
    QPushButton *confBtnDisconnect = nullptr;

    // methods
    ClientView(QWidget* = nullptr);
    void lstAddItem(QString, Client::ConnectionState);
    void lstRemoveItem(QString);
    void updateItem(QString, QString, Client::ConnectionState);
    QString currentItem();


signals:
    void sigAddNewClient();
    void sigRemoveClient(QString);
    void sigSelectClient(QString);
    void sigUnselectClient();
    void sigSaveClient(QString);
    void sigOpenPairWindow(QString);
    void sigDisconnectClient(QString);


private:
    // properties
    QListWidget *listView = nullptr;
    QLabel *warningClientId = nullptr;

    // methods
    QListWidgetItem* getItemById(QString);
    void keyPressEvent(QKeyEvent *event) override;
    static QString getIcon(Client::ConnectionState);
    static QString getToolTip(Client::ConnectionState);

    // nested classes
    class ClientIdValidator: public QRegExpValidator {
    public:
        ClientIdValidator(ClientView*);
        State validate(QString&, int&) const override;
        ClientView *cw = nullptr;
    };


};



#endif // CLIENTVIEW_H
