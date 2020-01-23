#include "client.h"



QHash<QString, QString> *Client::parseMessage (QString message) {
    // check if in correct format
    QRegularExpression reGlobal("^(\\w+=\"[^\"]+\";)+$");
    if (!reGlobal.match(message).hasMatch())
        return nullptr;

    // extract keys and values
    QRegularExpression reKVs("(\\w+)=\"([^\"]+)\";");
    QRegularExpressionMatchIterator i = reKVs.globalMatch(message);
    QHash<QString, QString> *map = new QHash<QString, QString>();
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        map->insert(match.captured(1), match.captured(2));
    }
    return map;
}



Client::Client (QString id, QSettings* sett, QObject *parent)
    : QObject(parent) {
    clientId = id;
    qSettings = sett;
}



Client::~Client() {
    if (currentPairing)
        currentPairing->deleteLater();
}



void Client::saveConfig () {
    // store results
    qSettings->beginGroup(QStringLiteral("clients/%1").arg(clientId));
    qSettings->setValue("active", active);
    qSettings->setValue("description", description);
    qSettings->setValue("askbeforeconnect", askBeforeConnect);
    qSettings->setValue("shownotificationonconnect", showNotificationOnConnect);
    qSettings->setValue("onlypluggedindevices", onlyPluggedInDevices);
    qSettings->setValue("secretkey", secretKey);
    qSettings->setValue("debugmode", debugmode);
    qSettings->endGroup();

    // remove old entry
    if (!origClientId.isEmpty()) {
        qSettings->remove(QStringLiteral("clients/%1").arg(origClientId));
        origClientId = "";
    }
}



void Client::loadConfig () {
    if (!origClientId.isEmpty())
        clientId = origClientId;
    qSettings->beginGroup(QStringLiteral("clients/%1").arg(clientId));
    active = qSettings->value("active", false).toBool();
    description = qSettings->value("description", "").toString();
    askBeforeConnect = qSettings->value("askbeforeconnect", false).toBool();
    showNotificationOnConnect = qSettings->value("shownotificationonconnect",
                                                 true).toBool();
    onlyPluggedInDevices = qSettings->value("onlypluggedindevices",
                                            true).toBool();
    secretKey = qSettings->value("secretkey").toByteArray();
    debugmode = qSettings->value("debugmode").toBool();
    qSettings->endGroup();
}



void Client::removeFromConfig () {
    QString key = QStringLiteral("clients/%1").arg(origClientId.isEmpty()
                                                   ? clientId : origClientId);
    qSettings->remove(key);
}



void Client::generateSecretKey () {
    QRandomGenerator *gen = QRandomGenerator::system();
    secretKey.clear();
    secretKey.resize(64);
    for (int dword = 0; dword < 8; dword++) {
        quint64 w = gen->generate64();
        for (int byte = 0; byte < 8; byte++)
            secretKey[(dword*8)+byte] = (w >> ((8-byte)*8)) & 0xFF;
    }
}



QByteArray Client::getSecretKey () {
    return secretKey;
}



QString Client::getId () {
    return origClientId.isEmpty() ? clientId : origClientId;
}



void Client::setId (QString id) {
    if (clientId == id)
        return;

    // save a backup for renaming in the settings
    if (origClientId.isEmpty())
        origClientId = clientId;
    clientId = id;

    // !!! update the client via network
}



Client::ConnectionState Client::getConnectionState() {
    if (isPaired())
        return E_PAIRED;
    else if (isActive())
        return E_ACTIVE;
    return E_INACTIVE;
}



bool Client::isActive () {
    return active;
}



void Client::setActive (bool newState) {
    active = newState;
    if (isPaired() && !newState)
        unpair();
}



bool Client::isOnlyPluggedInDevices () {
    return onlyPluggedInDevices;
}



void Client::setOnlyPluggedInDevices (bool newState) {
    onlyPluggedInDevices = newState;
    // !!! update the list of devices
}



bool Client::isPaired () {
    return currentPairing;
}



void Client::pair (QTcpSocket* socket, QString initialMessage, QByteArray hmac) {
    if (currentPairing || !socket)
        return;
    currentPairing = socket;
    processIncomingMessage(initialMessage, hmac);
}



void Client::unpair () {
    if (!isPaired())
        return;
    currentPairing->disconnect();
    currentPairing->close();
    currentPairing->deleteLater();
    currentPairing = nullptr;
    sigPairedChanged(this);
}



void Client::processIncomingTCPMessage () {
    if (!currentPairing)
        return;

    // receive input
    QString message = currentPairing->readLine();
    QByteArray hmac;
    if (currentPairing->bytesAvailable() >= 32)
        hmac = currentPairing->read(32);

    // do the proper signal processing
    processIncomingMessage(message, hmac);
}



void Client::processIncomingMessage (QString message, QByteArray hmac) {
    QHash<QString, QString> *map = parseMessage(message);
    if (!map)
        return;

    // check the timestamp and hmac
    if (!debugmode) {
        QDateTime time(QDateTime::currentDateTime());
        uint unixTime = time.toTime_t();
        if (qFabs(map->value("t", 0).toInt() - unixTime) > 30) {
            unpair();
            return;
        }

        QMessageAuthenticationCode correctHmac(QCryptographicHash::Sha256);
        correctHmac.setKey(secretKey);
        correctHmac.addData(message.toUtf8());
        QByteArray receivedHmac = hmac;
        if (correctHmac.result() != receivedHmac)
            // !!! Proper wrong hmac handling
            return;
    }

    // parse the actions
    QString action = map->value("a", "undefined");

    // initialize the connection
    if (action == "init") {
        // disconnect only signals
        currentPairing->disconnect();
        connect(currentPairing, &QTcpSocket::readyRead,
                this, &Client::processIncomingTCPMessage);
        connect(currentPairing, &QTcpSocket::disconnected,
                this, &Client::unpair);
        sigPairedChanged(this);
    }

    // test action
    else if (action == "test")
        sigCommanded("test");
}
