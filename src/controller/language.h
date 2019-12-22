#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QString>
#include <QList>



struct  Language {
    Language(QString, QString, bool);
    static QList<Language*> languages;
    QString nameShort;
    QString nameFull;
    bool isSelected;
};

#endif // LANGUAGE_H
