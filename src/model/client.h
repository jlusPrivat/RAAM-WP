#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QMessageAuthenticationCode>
#include <QRegularExpression>
#include <QDateTime>
#include <QtMath>



/**
 * \brief The Client class
 *
 * Represents a client (like a smartphone). They do not need to
 * be connected. This class also manages the network traffic with
 * this specific client.
 *
 * Registry using the {raam path}/clients/{client id}
 *
 * KEY                          DEF.    POSSIBLE
 * active                       false   true, false
 * description                  ""      All Strings
 * askbeforeconnect             false   true, false
 * shownotificationonconnect    true    true, false
 * onlypluggedindevices         true    true, false
 * secretkey                    ""      64-byte Byte array
 * debugmode                    false   true, false
 * */
class Client: public QObject {
    Q_OBJECT


public:
    // enumerations
    enum Connectionerror {
    };

    // enumerations
    enum ConnectionState {E_PAIRED, E_ACTIVE, E_INACTIVE};

    // properties
    QString description;
    bool askBeforeConnect = false;
    bool showNotificationOnConnect = true;
    bool debugmode = false;

    // methods
    /// parses a message into its contents; Null, when not parsable
    static QHash<QString, QString> *parseMessage(QString);
    Client(QString, QSettings*, QObject* = nullptr);
    ~Client();
    /// Pushes the client config for permanent storage.
    void saveConfig();
    /// Loads the client config from storage based on its id.
    /// All overriden configurations will be ignored.
    void loadConfig();
    /// removes this client from permanent storage.
    void removeFromConfig();
    void generateSecretKey();
    QByteArray getSecretKey();
    QString getId();
    void setId(QString);
    ConnectionState getConnectionState();
    bool isActive();
    void setActive(bool);
    bool isOnlyPluggedInDevices();
    void setOnlyPluggedInDevices(bool);
    bool isPaired();


signals:
    void sigPairedChanged(Client*);
    /// triggers, when any command is validated and triggered by the client
    void sigCommanded(QString); // !!! Add command here
    void sigErrored(Connectionerror);


public slots:
    void pair(QTcpSocket*, QString, QByteArray);
    /// will properly close the connection
    void unpair();
    /// will process any incoming message
    void processIncomingTCPMessage();
    /// processes any message and validates the byte array
    void processIncomingMessage(QString, QByteArray);


private:
    // properties
    QSettings *qSettings = nullptr;
    QString clientId;
    QString origClientId = "";
    bool active = true;
    QTcpSocket *currentPairing = nullptr;
    bool onlyPluggedInDevices = true;
    QByteArray secretKey;


};



#endif // CLIENT_H
