#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QMessageBox>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStatusBar>
#include "view/clientview.h"
#include "view/settingsview.h"
#include "view/aboutview.h"



class MainWindow: public QMainWindow {
    Q_OBJECT


public:
    ClientView *clientTab = nullptr;
    SettingsView *settingsTab = nullptr;
    QStatusBar *statusbar = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showMessage(QString, bool);
    bool askForRestart();
    bool askForConnect(QString);


private:
    void closeEvent(QCloseEvent*);


signals:
    void sigCloseRequested(bool);
    void sigTrayOpenApp();
    void sigTrayCloseApp();


};

#endif // MAINWINDOW_H
