#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStatusBar>
#include "view/settings.h"
#include "view/about.h"



class MainWindow: public QMainWindow {
    Q_OBJECT


public:
    Settings *settingsTab = nullptr;
    QStatusBar *statusbar = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool askForRestart();


private:
    void closeEvent(QCloseEvent*);


signals:
    void closeRequested(bool);
    void trayOpenApp();
    void trayCloseApp();


};

#endif // MAINWINDOW_H
