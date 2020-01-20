#include "client.h"



Client::Client (QString id, QSettings* sett, QObject *parent)
    : QObject(parent) {
    clientId = id;
    qSettings = sett;
}



Client::~Client() {
    unpair(); // !!!
}



void Client::saveConfig () {
    // store results
    qSettings->beginGroup(QStringLiteral("clients/%1").arg(clientId));
    qSettings->setValue("active", active);
    qSettings->setValue("description", description);
    qSettings->setValue("askBeforeConnect", askBeforeConnect);
    qSettings->setValue("showNotificationOnConnect", showNotificationOnConnect);
    qSettings->setValue("onlyPluggedInDevices", onlyPluggedInDevices);
    qSettings->setValue("secretKey", secretKey);
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
    askBeforeConnect = qSettings->value("askBeforeConnect", false).toBool();
    showNotificationOnConnect = qSettings->value("showNotificationOnConnect",
                                                 true).toBool();
    onlyPluggedInDevices = qSettings->value("onlyPluggedInDevices",
                                            true).toBool();
    secretKey = qSettings->value("secretKey").toByteArray();
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
    return currentParing;
}



void Client::pair (QTcpSocket* socket, QString initialMessage) {
    currentParing = socket;
    // !!! has to load 32 byte hmac from socket
    // !!! Parse initial message and check for integrity of hmac
}



void Client::unpair () {
    if (!isPaired())
        return;
    // !!!
}
