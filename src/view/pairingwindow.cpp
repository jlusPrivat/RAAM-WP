#include "pairingwindow.h"



PairingWindow::PairingWindow(QWidget *parent)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint) {
    // configure the window
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMinimumSize(450, 300);
    QHBoxLayout *layout = new QHBoxLayout(this);


    // the qr code side
    QGroupBox *wGroupQr = new QGroupBox(this);
    wGroupQr->setTitle(tr("Pair using Qr Code"));
    layout->addWidget(wGroupQr);
    // !!!


    // the raw information side
    QGroupBox *wGroupRaw = new QGroupBox(this);
    wGroupRaw->setTitle(tr("Pair using raw information"));
    layout->addWidget(wGroupRaw);
    QFormLayout *lGroupRaw = new QFormLayout(wGroupRaw);

    // the description for the raw information
    QLabel *wDescriptionRaw = new QLabel(tr("Use the information below to manually "
                                            "connect to this server using your "
                                            "client device. You can copy all fields."),
                                         this);
    wDescriptionRaw->setWordWrap(true);
    lGroupRaw->addRow(wDescriptionRaw);

    // the server id
    wServerId = new QLabel(wGroupRaw);
    wServerId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wServerId->setStyleSheet("background-color: white; padding: 2px");
    lGroupRaw->addRow(tr("Server ID:"), wServerId);

    // the server ip
    wServerIp = new QLabel(wGroupRaw);
    wServerIp->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wServerIp->setStyleSheet("background-color: white; padding: 2px");
    lGroupRaw->addRow(tr("Client IP:"), wServerIp);

    // the server port
    wServerPort = new QLabel(wGroupRaw);
    wServerPort->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wServerPort->setStyleSheet("background-color: white; padding: 2px");
    lGroupRaw->addRow(tr("Server Port:"), wServerPort);

    // the client id
    wClientId = new QLabel(wGroupRaw);
    wClientId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wClientId->setStyleSheet("background-color: white; padding: 2px");
    lGroupRaw->addRow(tr("Client ID:"), wClientId);

    // the client secret
    wClientSecret = new QLabel(wGroupRaw);
    wClientSecret->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wClientSecret->setWordWrap(true);
    wClientSecret->setStyleSheet("background-color: white; padding: 2px");
    lGroupRaw->addRow(tr("Client Secret:"), wClientSecret);
}



void PairingWindow::setClientId (QString id) {
    setWindowTitle(tr("Paring configuration for \"%1\"").arg(id));
    wClientId->setText(id);
}



void PairingWindow::generateQr (QByteArray) {

}



void PairingWindow::show () {
    QWidget::show();
}



void PairingWindow::changeEvent (QEvent *event) {
    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow())
            close();
    }
}
