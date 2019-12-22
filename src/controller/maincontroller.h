#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include "controller/language.h"
#include "view/mainwindow.h"



class MainController: public QObject {
    Q_OBJECT


public:
    MainController(QObject* = nullptr);


private:
    MainWindow *w = nullptr;
    QTranslator *translator = new QTranslator();
    QSettings *qSettings = nullptr;

    void updateLanguageController(QString);


private slots:
    void updateSettings();


};

#endif // MAINCONTROLLER_H
