#include "language.h"



QList<Language*> Language::languages = {
    new Language("de", "Deutsch", false),
    new Language("en", "English", false)
};



Language::Language (QString shor, QString full, bool selected) {
    nameShort = shor;
    nameFull = full;
    isSelected = selected;
}
