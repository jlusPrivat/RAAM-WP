#include "clientview.h"



ClientView::ClientView (QWidget *parent)
    : QWidget(parent) {
    // config the main widget
    setBackgroundRole(QPalette::Light);
    QHBoxLayout *layout = new QHBoxLayout(this);


    // add the list widget
    QWidget *wList = new QWidget(this);
    layout->addWidget(wList);
    QGridLayout *lList = new QGridLayout(wList);
    lList->setContentsMargins(0, 0, 0, 0);

    // add the list view
    listView = new QListWidget(this);
    listView->setFixedWidth(200);
    listView->setMovement(QListView::Movement::Static);
    listView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    connect(listView, &QListWidget::itemSelectionChanged, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            sigSelectClient(item->text());
        else
            sigUnselectClient();
    });
    lList->addWidget(listView, 0, 0, 1, 2);

    // add the add button
    listBtnAdd = new QPushButton(QIcon(":/imgs/add-icon.png"),
                                 tr("New"), wList);
    connect(listBtnAdd, &QPushButton::clicked, this, &ClientView::sigAddNewClient);
    lList->addWidget(listBtnAdd, 1, 0);

    // add the delete button
    listBtnRemove = new QPushButton(QIcon(":/imgs/delete-icon.png"),
                                    tr("Delete"), wList);
    listBtnRemove->setDisabled(true);
    connect(listBtnRemove, &QPushButton::clicked, this, [&]{
        sigRemoveClient(currentItem());
    });
    lList->addWidget(listBtnRemove, 1, 1);


    // add the right sided scrollbar
    QScrollArea *wConfScroll = new QScrollArea(this);
    wConfScroll->setFrameShape(QFrame::NoFrame);
    wConfScroll->setWidgetResizable(true);
    wConfScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    wConfScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(wConfScroll);
    QVBoxLayout *lConfScroll = new QVBoxLayout(wConfScroll);

    // add the right sided dialog
    QGroupBox *wConfBox = new QGroupBox(wConfScroll);
    wConfBox->setTitle(tr("Client configuration"));
    wConfScroll->setWidget(wConfBox);

    // add the conf box
    wConfDiag = new QWidget(wConfBox);
    lConfScroll->addWidget(wConfDiag);
    QFormLayout *lConfDiag = new QFormLayout(wConfDiag);
    lConfDiag->setSpacing(12);
    lConfDiag->setContentsMargins(4, 12, 4, 4);

    // add client ID and its icon
    QWidget *wConfClientId = new QWidget(wConfDiag);
    lConfDiag->addRow(tr("Client ID:"), wConfClientId);
    QHBoxLayout *lConfClientId = new QHBoxLayout(wConfClientId);
    lConfClientId->setContentsMargins(0, 0, 0, 0);
    // client id field
    confClientId = new QLineEdit(wConfClientId);
    confClientId->setValidator(new ClientIdValidator(this));
    lConfClientId->addWidget(confClientId);
    // warning icon
    QPixmap pmWarningIcon(":/imgs/warning.png");
    warningClientId = new QLabel(wConfClientId);
    warningClientId->setPixmap(pmWarningIcon);
    warningClientId->setToolTip(tr("- 3 to 100 characters allowed\n"
                                   "- Only lowercase alphanumeric, space "
                                   "and underscore\n"
                                   "- Beginning only letters\n"
                                   "- No identical other client id"));
    warningClientId->setVisible(false);
    lConfClientId->addWidget(warningClientId);
    connect(confClientId, &QLineEdit::textChanged, this, [&]{
        warningClientId->setVisible(!confClientId->hasAcceptableInput());
    });

    // add active
    confActive = new QCheckBox(wConfDiag);
    confActive->setText(tr("If not set, the client will not be able to pair"));
    lConfDiag->addRow(tr("Enabled:"), confActive);

    // add description
    confDescription = new QPlainTextEdit(wConfDiag);
    confDescription->setFixedHeight(60);
    lConfDiag->addRow(tr("Description:"), confDescription);

    // add ask before connecting
    confAskPairing = new QCheckBox(wConfDiag);
    confAskPairing->setText(tr("Ask before pairing"));
    lConfDiag->addRow(tr("Ask Pairing:"), confAskPairing);

    // add notification upon pairing
    confShowNotification = new QCheckBox(wConfDiag);
    confShowNotification->setText(tr("Show notification when pairing"));
    lConfDiag->addRow(tr("Notify Pairing:"), confShowNotification);

    // add only plugged in devices
    confOnlyPluggedIn = new QCheckBox(wConfDiag);
    confOnlyPluggedIn->setText(tr("Only plugged in Audio Endpoints will be visible"));
    lConfDiag->addRow(tr("Only Plugged:"), confOnlyPluggedIn);

    // add debug mode
    confDebugMode = new QCheckBox(wConfDiag);
    confDebugMode->setText(tr("For usage with raw TCP/BLE messages"));
    confDebugMode->setToolTip(tr("No HMAC will be generated, instead\n"
                                 "a newline character is appended.\n"
                                 "No HMACS or timestamps will be validated."));
    lConfDiag->addRow(tr("Debug mode:"), confDebugMode);


    // add the right sided buttons
    QWidget *wConfBtns = new QWidget(wConfDiag);
    QHBoxLayout *lConfBtns = new QHBoxLayout(wConfBtns);
    lConfBtns->setContentsMargins(0, 0, 0, 0);
    lConfDiag->addRow(wConfBtns);

    // the save button
    confBtnSave = new QPushButton(tr("Save"), wConfBtns);
    confBtnSave->setDefault(true);
    connect(confClientId, &QLineEdit::textChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confActive, &QCheckBox::stateChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confDescription, &QPlainTextEdit::textChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confAskPairing, &QCheckBox::stateChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confShowNotification, &QCheckBox::stateChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confOnlyPluggedIn, &QCheckBox::stateChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confDebugMode, &QCheckBox::stateChanged,
            this, [&]{confBtnSave->setDisabled(false);});
    connect(confBtnSave, &QPushButton::clicked, this, [&]{
        sigSaveClient(currentItem());
    });
    lConfBtns->addWidget(confBtnSave);

    // the reset button
    confBtnReset = new QPushButton(tr("Reset"), wConfBtns);
    connect(confBtnReset, &QPushButton::clicked, this, [&]{
        sigSelectClient(currentItem());
    });
    lConfBtns->addWidget(confBtnReset);

    // the pair button
    confBtnPair = new QPushButton(tr("Pair"), wConfBtns);
    connect(confBtnPair, &QPushButton::clicked, this, [&]{
        sigOpenPairWindow(currentItem());
    });
    lConfBtns->addWidget(confBtnPair);

    // the disconnect button
    confBtnDisconnect = new QPushButton(tr("Disconnect"), wConfBtns);
    connect(confBtnDisconnect, &QPushButton::clicked, this, [&]{
        sigDisconnectClient(currentItem());
    });
    lConfBtns->addWidget(confBtnDisconnect);

    // disable the entire boxgroup
    wConfDiag->setDisabled(true);
}



void ClientView::lstAddItem (QString id, Client::ConnectionState state) {
    QListWidgetItem *item = new QListWidgetItem(QIcon(getIcon(state)),
                                                id, listView);
    item->setToolTip(getToolTip(state));
    listView->sortItems();
}



void ClientView::lstRemoveItem (QString id) {
    QListWidgetItem *item = getItemById(id);
    if (item) {
        listView->removeItemWidget(item);
        delete item;
    }
}



void ClientView::updateItem (QString oldId, QString newId,
                             Client::ConnectionState state) {
    QListWidgetItem *item = getItemById(oldId);
    if (item) {
        item->setText(newId);
        item->setIcon(QIcon(getIcon(state)));
        item->setToolTip(getToolTip(state));
        listView->sortItems();
    }
}



QString ClientView::currentItem () {
    QListWidgetItem *item = listView->currentItem();
    if (!item)
        return "";
    return item->text();
}



void ClientView::keyPressEvent (QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        confBtnSave->click();
        event->accept();
    }
}



QString ClientView::getIcon (Client::ConnectionState state) {
    switch (state) {
    case Client::E_PAIRED:
        return ":/imgs/paired-icon.png";
    case Client::E_ACTIVE:
        return ":/imgs/active-icon.png";
    default:
        return ":/imgs/inactive-icon.png";
    }
}



QString ClientView::getToolTip (Client::ConnectionState state) {
    switch (state) {
    case Client::E_PAIRED:
        return tr("Paired");
    case Client::E_ACTIVE:
        return tr("Waiting for pairing");
    default:
        return tr("Inactive");
    }
}



QListWidgetItem* ClientView::getItemById (QString id) {
    auto list = listView->findItems(id, Qt::MatchFlag::MatchExactly);
    if (list.count() != 1)
        return nullptr;
    return list.at(0);
}



ClientView::ClientIdValidator::ClientIdValidator (ClientView *parent)
    : QRegExpValidator(parent) {
    QRegExp rx("[a-z][a-z0-9 _\\-]{1,23}[a-z0-9]");
    setRegExp(rx);
    cw = parent;
}



QValidator::State ClientView::ClientIdValidator::validate
(QString &str, int &pos) const {
    State precheck = QRegExpValidator::validate(str, pos);
    if (precheck != Acceptable || !cw->getItemById(str)
            || str == cw->currentItem())
        return precheck;
    return Invalid;
}
