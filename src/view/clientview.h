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



class ClientView: public QScrollArea {
    Q_OBJECT


public:
    // enumerations
    enum IconType {E_ICON_PAIRED, E_ICON_ACTIVE, E_ICON_INACTIVE};

    // properties
    QPushButton *listBtnAdd = nullptr;
    QPushButton *listBtnRemove = nullptr;

    QGroupBox *confBox = nullptr;
    QLineEdit *confClientId = nullptr;
    QCheckBox *confActive = nullptr;
    QPlainTextEdit *confDescription = nullptr;
    QCheckBox *confAskPairing = nullptr;
    QCheckBox *confShowNotification = nullptr;
    QCheckBox *confOnlyPluggedIn = nullptr;
    QPushButton *confBtnSave = nullptr;
    QPushButton *confBtnReset = nullptr;
    QPushButton *confBtnPair = nullptr;
    QPushButton *confBtnDisconnect = nullptr;

    // methods
    ClientView(QWidget* = nullptr);
    void lstAddItem(QString, IconType);
    void lstRemoveItem(QString);
    void updateItem(QString, QString, IconType);


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
    QString getIcon(IconType);

    // nested classes
    class ClientIdValidator: public QRegExpValidator {
    public:
        ClientIdValidator(ClientView*);
        State validate(QString&, int&) const override;
        ClientView *cw = nullptr;
    };


};



#endif // CLIENTVIEW_H
