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
    listView = new QListWidget(this);
    listView->setFixedWidth(200);
    listView->setMovement(QListView::Movement::Static);
    listView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    connect(listView, &QListWidget::itemSelectionChanged, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            selectClient(item->text());
        else
            unselectClient();
    });
    lList->addWidget(listView, 0, 0, 1, 2);

    // add the add button
    listBtnAdd = new QPushButton(QIcon(":/imgs/add-icon.png"),
                                 tr("New"), wList);
    connect(listBtnAdd, &QPushButton::clicked, this, &ClientView::addNewClient);
    lList->addWidget(listBtnAdd, 1, 0);

    // add the delete button
    listBtnRemove = new QPushButton(QIcon(":/imgs/delete-icon.png"),
                                    tr("Delete"), wList);
    listBtnRemove->setDisabled(true);
    connect(listBtnRemove, &QPushButton::clicked, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            removeClient(item->text());
    });
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
    connect(confBtnSave, &QPushButton::clicked, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            saveClient(item->text());
    });
    lConfBtns->addWidget(confBtnSave);

    // the reset button
    confBtnReset = new QPushButton(tr("Reset"), wConfBtns);
    connect(confBtnReset, &QPushButton::clicked, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            selectClient(item->text());
    });
    lConfBtns->addWidget(confBtnReset);

    // the pair button
    confBtnPair = new QPushButton(tr("Pair"), wConfBtns);
    connect(confBtnPair, &QPushButton::clicked, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            openPairWindow(item->text());
    });
    lConfBtns->addWidget(confBtnPair);

    // the disconnect button
    confBtnDisconnect = new QPushButton(tr("Disconnect"), wConfBtns);
    connect(confBtnReset, &QPushButton::clicked, this, [&]{
        QListWidgetItem *item = listView->currentItem();
        if (item)
            disconnectClient(item->text());
    });
    lConfBtns->addWidget(confBtnDisconnect);

    // disable the entire boxgroup
    confBox->setDisabled(true);
}



void ClientView::lstAddItem (QString id, IconType icon) {
    new QListWidgetItem(QIcon(getIcon(icon)), id, listView);
    listView->sortItems();
}



void ClientView::lstRemoveItem (QString id) {
    QListWidgetItem *item = getItemById(id);
    if (item) {
        listView->removeItemWidget(item);
        delete item;
    }
}



void ClientView::updateItem (QString oldId, QString newId, IconType icon) {
    QListWidgetItem *item = getItemById(oldId);
    if (item) {
        item->setText(newId);
        item->setIcon(QIcon(getIcon(icon)));
        listView->sortItems();
    }
}



QString ClientView::getIcon (IconType t) {
    switch (t) {
    case E_ICON_PAIRED:
        return ":/imgs/paired-icon.png";
    case E_ICON_ACTIVE:
        return ":/imgs/active-icon.png";
    default:
        return ":/imgs/inactive-icon.png";
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
            || str == cw->listView->currentItem()->text())
        return precheck;
    return Invalid;
}
