#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QString>
#include <QList>



/**
 * \brief The Language struct
 *
 * Keeps an enumeration of all available languages and
 * is aware of the current setting.
 */
struct  Language {
    Language(QString, QString, bool);
    static QList<Language*> languages;
    QString nameShort;
    QString nameFull;
    bool isSelected;
};

#endif // LANGUAGE_H
