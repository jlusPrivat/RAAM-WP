#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QTcpSocket>



/* Registry using the {raam path}/clients/{client id}
 *
 * KEY                          DEF.    POSSIBLE
 * active                       false   true, false
 * description                  ""      All Strings
 * askBeforeConnect             false   true, false
 * showNotificationOnConnect    true    true, false
 * onlyPluggedInDevices         true    true, false
 * */
class Client: public QObject {
    Q_OBJECT


public:
    // enumerations
    enum Connectionerror {
    };

    // properties
    QString description;
    bool askBeforeConnect = false;
    bool showNotificationOnConnect = true;

    // methods
    Client(QString, QSettings*, QObject* = nullptr);
    ~Client();
    void saveConfig();
    void loadConfig();
    void removeFromConfig();
    QString getId();
    void setId(QString);
    bool isActive();
    void setActive(bool);
    bool isOnlyPluggedInDevices();
    void setOnlyPluggedInDevices(bool);
    bool isPaired();


signals:
    void sigPaired();
    void sigUnpaired();
    void sigCommanded(); // Add command here
    void sigErrored(Connectionerror);


public slots:
    void pair(QTcpSocket*);
    void unpair();


private:
    // properties
    QSettings *qSettings = nullptr;
    QString clientId;
    QString origClientId = "";
    bool active = true;
    QTcpSocket *currentParing = nullptr;
    bool onlyPluggedInDevices = true;


};



#endif // CLIENT_H
