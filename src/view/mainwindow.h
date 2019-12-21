#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include "view/settings.h"
#include "view/about.h"



class MainWindow: public QMainWindow {
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStatusBar *statusbar;


};

#endif // MAINWINDOW_H
