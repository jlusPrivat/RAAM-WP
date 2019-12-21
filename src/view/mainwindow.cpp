#include "mainwindow.h"



MainWindow::MainWindow (QWidget *parent)
    : QMainWindow(parent) {
    // set the main windows config
    this->setObjectName("mainWindow");
    this->setWindowTitle("Remote Application Audio Mixer");
    this->setMinimumSize(600, 300);

    // add the TabWidget
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *centralLayout = new QGridLayout(centralWidget);
    QTabWidget *tabBar = new QTabWidget(centralWidget);
    centralLayout->setMargin(5);
    centralLayout->addWidget(tabBar);
    setCentralWidget(centralWidget);

    // add the specific tabs
    tabBar->addTab(new Settings(this), tr("Settings"));
    tabBar->addTab(new About(this), tr("About"));

    // add statusbar
    statusbar = new QStatusBar(this);
    this->setStatusBar(statusbar);
}



MainWindow::~MainWindow() {
}
