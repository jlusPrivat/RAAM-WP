#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QHash>
#include <QDateTime>



class Command: public QObject {


public:
    // enumerations
    enum Direction {
        E_INBOUND = 0x01, E_OUTBOUND = 0x02, E_ANY = 0x03
    };

    // properties
    Direction direction;

    // methods
    Command(QString = "", Direction = E_ANY, QObject* = nullptr);
    /// checks, if the command may be valid, without timestamp or contextual analysis
    bool isValid();
    /// gets the defined action, empty string if not set
    QString getAction();
    /// sets the action
    void setAction(QString);
    /// serializes the command for transmission.
    /// Set to true for adding current timestamp.
    QString serialize(bool);
    /// true, if key is contained and a value is set. Also returns false,
    /// when it contains a " (quote) character
    bool containsKey(QString);
    /// outputs a the value to a given key. Empty string if not set.
    QString value(QString);
    /// puts a key value combination into the command
    void put(QString, QString);
    void put(QString, int);
    /// puts many key value combinations into the command
    void putAll(QHash<QString, QString>&);


private:
    // properties
    QHash<QString, QString> map;


};



#endif // COMMAND_H
