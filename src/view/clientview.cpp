#include "clientview.h"



ClientView::ClientView (QWidget *parent)
    : QScrollArea(parent) {
    // config the main widget
    setBackgroundRole(QPalette::Light);
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // configure the layout
    QWidget *centralWidget = new QWidget(this);
    setWidget(centralWidget);
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);


    // add the list widget
    QWidget *wList = new QWidget(this);
    layout->addWidget(wList);
    QGridLayout *lList = new QGridLayout(wList);
    lList->setMargin(0);

    // add the list view
    listView = new QListView(this);
    listView->setFixedWidth(200);
    lList->addWidget(listView, 0, 0, 1, 2);

    // add the add button
    listBtnAdd = new QPushButton(QIcon(":/imgs/add-icon.png"),
                                 tr("New"), wList);
    connect(listBtnAdd, &QPushButton::clicked, this, &ClientView::addNewClient);
    lList->addWidget(listBtnAdd, 1, 0);

    // add the delete button
    listBtnRemove = new QPushButton(QIcon(":/imgs/delete-icon.png"),
                                    tr("Delete"), wList);
    connect(listBtnRemove, &QPushButton::clicked, this, &ClientView::removeClient);
    lList->addWidget(listBtnRemove, 1, 1);


    // add the conf box
    confBox = new QGroupBox(this);
    layout->addWidget(confBox);
    confBox->setTitle(tr("Client configuration"));
    QFormLayout *confLayout = new QFormLayout(confBox);
    confLayout->setSpacing(12);

    // add client ID and its icon
    QWidget *wConfClientId = new QWidget(confBox);
    confLayout->addRow(tr("Client ID:"), wConfClientId);
    QHBoxLayout *lConfClientId = new QHBoxLayout(wConfClientId);
    lConfClientId->setMargin(0);
    // client id field
    confClientId = new QLineEdit(wConfClientId);
    lConfClientId->addWidget(confClientId);
    // warning icon
    QPixmap pmWarningIcon(":/imgs/warning.png");
    warningClientId = new QLabel(wConfClientId);
    warningClientId->setPixmap(pmWarningIcon);
    warningClientId->setToolTip(tr("- 3 to 100 characters allowed\n"
                                   "- Only alphanumeric, space and underscore\n"
                                   "- Beginning only letters"));
    warningClientId->setVisible(true); // !!!
    lConfClientId->addWidget(warningClientId);
    connect(confClientId, &QLineEdit::textChanged, this, [&]{
        warningClientId->setVisible(!confClientId->hasAcceptableInput());
    });

    // add active
    confActive = new QCheckBox(confBox);
    confActive->setText(tr("If not set, the client will not be able to pair"));
    confLayout->addRow(tr("Enabled:"), confActive);

    // add description
    confDescription = new QPlainTextEdit(confBox);
    confDescription->setFixedHeight(60);
    confLayout->addRow(tr("Description:"), confDescription);

    // add ask before connecting
    confAskPairing = new QCheckBox(confBox);
    confAskPairing->setText(tr("Ask before pairing"));
    confLayout->addRow(tr("Ask Pairing:"), confAskPairing);

    // add notification upon pairing
    confShowNotification = new QCheckBox(confBox);
    confShowNotification->setText(tr("Show notification when pairing"));
    confLayout->addRow(tr("Notify Pairing:"), confShowNotification);

    // add only plugged in devices
    confOnlyPluggedIn = new QCheckBox(confBox);
    confOnlyPluggedIn->setText(tr("Only plugged in Audio Endpoints will be visible"));
    confLayout->addRow(tr("Only Plugged:"), confOnlyPluggedIn);


    // add the right sided buttons
    QWidget *wConfBtns = new QWidget(confBox);
    QHBoxLayout *lConfBtns = new QHBoxLayout(wConfBtns);
    lConfBtns->setMargin(0);
    confLayout->addRow(wConfBtns);

    // the save button
    confBtnSave = new QPushButton(tr("Save"), wConfBtns);
    confBtnSave->setDefault(true);
    connect(confBtnSave, &QPushButton::clicked, this, &ClientView::saveClient);
    lConfBtns->addWidget(confBtnSave);

    // the reset button
    confBtnReset = new QPushButton(tr("Reset"), wConfBtns);
    lConfBtns->addWidget(confBtnReset);

    // the pair button
    confBtnPair = new QPushButton(tr("Pair"), wConfBtns);
    lConfBtns->addWidget(confBtnPair);

    // the disconnect button
    confBtnDisconnect = new QPushButton(tr("Disconnect"), wConfBtns);
    lConfBtns->addWidget(confBtnDisconnect);
}
