#include "mainwindow.h"



MainWindow::MainWindow (QWidget *parent)
    : QMainWindow(parent) {
    // set the main windows config
    this->setObjectName("mainWindow");
    this->setWindowTitle("Remote Application Audio Mixer");
    this->setMinimumSize(600, 300);

    // add the TabWidget and statusbar
    QTabWidget *tabBar = new QTabWidget(this);
    this->setCentralWidget(tabBar);
    statusbar = new QStatusBar(this);
    this->setStatusBar(statusbar);

    // add the specific tabs
    tabBar->addTab(new Settings(this), tr("Settings"));
    tabBar->addTab(new About(this), tr("About"));
}



MainWindow::~MainWindow() {
}
