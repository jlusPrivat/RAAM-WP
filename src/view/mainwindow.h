#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTabWidget>
#include <QStatusBar>
#include "view/settings.h"
#include "view/about.h"



class MainWindow: public QMainWindow {
    Q_OBJECT


public:
    Settings *settingsTab = nullptr;
    QStatusBar *statusbar = nullptr;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool askForRestart();


};

#endif // MAINWINDOW_H
