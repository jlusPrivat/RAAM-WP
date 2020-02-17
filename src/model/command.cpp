#include "command.h"



Command::Command (QString action, Direction dir, QObject* parent)
    : QObject(parent), direction(dir) {
    setAction(action);
}



bool Command::isValid () {
    // Inbound direction
    if (direction & E_INBOUND) {
        if (getAction() == "init" && containsKey("c"))
            return true;
        if (getAction() == "inforeturn" && containsKey("c")
                && containsKey("sw") && containsKey("v") && containsKey("sv"))
            return true;
        if (getAction() == "enumDevices")
            return true;
        if (getAction() == "dev" && containsKey("di"))
            return true;
        if (getAction() == "enumSessions" && containsKey("di"))
            return true;
        if (getAction() == "sess" && containsKey("si") && containsKey("di"))
            return true;
    }

    // Outbound direction
    if (direction & E_OUTBOUND) {
        if (getAction() == "init" && containsKey("s"))
            return true;
        if (getAction() == "changeServerId" && containsKey("s"))
            return true;
        if (getAction() == "changeClientId" && containsKey("c"))
            return true;
        if (getAction() == "inforeturn" && containsKey("s") && containsKey("c")
                && containsKey("sw") && containsKey("v") && containsKey("sv"))
            return true;
        if (getAction() == "enumDevices")
            return true;
        if (getAction() == "dev" && containsKey("di") && containsKey("dc"))
            return true;
        if (getAction() == "enumSessions" && containsKey("di"))
            return true;
        if (getAction() == "sess" && containsKey("si") && containsKey("di"))
            return true;
    }

    // Any direction
    if (getAction() == "info")
        return true;

    return false;
}



QString Command::getAction () {
    return map.value("a", "");
}



void Command::setAction (QString action) {
    map.insert("a", action);
}



QString Command::serialize (bool addTimestamp) {
    if (addTimestamp) {
        QDateTime asd(QDateTime::currentDateTime());
        put("t", asd.toTime_t());
    }

    QString rv;
    QList<QString> keys = map.keys();
    int count = keys.count();
    for (int i = 0; i < count; i++) {
        QString k = keys.at(i);
        rv.append(k + "=\"" + map.value(k) + "\";");
    }
    return rv;
}



bool Command::containsKey (QString key) {
    if (!map.contains(key) || map.value(key).contains('"'))
        return false;
    return true;
}



QString Command::value (QString key) {
    return map.value(key);
}



void Command::put (QString key, QString val) {
    map.insert(key, val);
}



void Command::put (QString key, int val) {
    map.insert(key, QString::number(val));
}



void Command::putAll (QHash<QString, QString> &otherMap) {
    map = otherMap;
}



Command& Command::operator<< (QString key) {
    map.insert(key, "");
    return *this;
}
