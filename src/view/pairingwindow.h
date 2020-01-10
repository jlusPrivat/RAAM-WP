#ifndef PAIRINGWINDOW_H
#define PAIRINGWINDOW_H

#include <QDialog>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QEvent>



/**
 * \brief The PairingWindow class
 *
 * displays the information for pairing with
 * a client.
 * This widget deletes itself from heap,
 * after it has been closed.
 * All properties should be assigned a text
 * for display
 * Call show method, after filling all properties.
*/
class PairingWindow: QDialog {
    Q_OBJECT


public:
    // properties
    QLabel *wServerId = nullptr;
    QLabel *wServerIp = nullptr;
    QLabel *wServerPort = nullptr;
    QLabel *wClientSecret = nullptr;

    // methods
    PairingWindow(QWidget* = nullptr);
    void setClientId(QString);
    /// pass the correctly assembled code as of specs
    void generateQr(QByteArray);
    void show();


private:
    QLabel *wClientId = nullptr;
    void changeEvent(QEvent*) override;


};



#endif // PAIRINGWINDOW_H
