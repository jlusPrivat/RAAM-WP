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
    tabBar->addTab(settingsTab = new Settings(this), tr("Settings"));
    tabBar->addTab(new About(this), tr("About"));

    // add statusbar
    statusbar = new QStatusBar(this);
    this->setStatusBar(statusbar);
}



MainWindow::~MainWindow () {
}



bool MainWindow::askForRestart () {
    // configure the message
    QMessageBox modal(QMessageBox::Question, tr("Restart RAAM?"),
                      tr("You have changed settings that require a restart to "
                         "take effect. Do you want to restart now?"),
                      QMessageBox::NoButton, this);
    QPushButton *buttonYes = modal.addButton(tr("Yes"), QMessageBox::YesRole);
    modal.addButton(tr("No"), QMessageBox::NoRole);
    modal.setDefaultButton(buttonYes);

    // display the message
    modal.exec();

    // check the result
    if (modal.clickedButton() == buttonYes)
        return true;
    return false;
}
