#ifndef SETTINGS_H
#define SETTINGS_H

#include <QScrollArea>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>



class Settings: public QScrollArea {
    Q_OBJECT


public:
    Settings(QWidget*);


private:
    QCheckBox *keepInTray = new QCheckBox(this);

};

#endif // SETTINGS_H
