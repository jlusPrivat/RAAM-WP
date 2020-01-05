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
    qSettings->endGroup();
}



void Client::removeFromConfig () {
    QString key = QStringLiteral("clients/%1").arg(origClientId.isEmpty()
                                                   ? clientId : origClientId);
    qSettings->remove(key);
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



void Client::pair (QTcpSocket* socket) {
    currentParing = socket;
    // !!!
}



void Client::unpair () {
    if (!isPaired())
        return;
    // !!!
}
