#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QObject>
#include <QScrollArea>
#include <QGroupBox>
#include <QListView>
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
    // properties
    QListView *listView = nullptr;
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


signals:
    void addNewClient();
    void removeClient();
    void saveClient();
    void selectClient(QString);
    void openPairWindow(QString);
    void disconnectClient(QString);


private:
    QLabel *warningClientId = nullptr;


};



#endif // CLIENTVIEW_H
