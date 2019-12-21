#include "about.h"



About::About (QWidget *parent)
    : QScrollArea(parent) {
    setBackgroundRole(QPalette::Light);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    QWidget *content = new QWidget();
    content->setLayout(layout);
    setWidget(content);

    QLabel *image = new QLabel(this);
    image->setAlignment(Qt::AlignTop);
    image->setMargin(20);
    layout->addWidget(image);
    QPixmap pm(":/imgs/logo.png");
    image->setPixmap(pm);

    QLabel *lbl = new QLabel(this);
    layout->addWidget(lbl);
    lbl->setWordWrap(true);
    lbl->setTextFormat(Qt::RichText);
    lbl->setOpenExternalLinks(true);
    lbl->setText(tr("<b>Remote Application Audio Program</b> "
                    "(RAAM-WP), developed by <a href=\"https://jlus.de\">"
                    "Jonathan Lusky</a> as <a href=\"https://example.org\">"
                    "bachelor's thesis</a>."
                    "<br><br><b>Overview</b><br>"
                    "RAAM is a collection of softwares that enable users to "
                    "remotely control audio settings via a network connection. "
                    "It may be useful for Windows users, who intend to level "
                    "their games volume with the volume of a communication "
                    "software without switching to sndvol. RAAM is devided "
                    "into multiple softwares:<ul>"
                    "<li><b>RAAM-WP</b>: RAAM - Windows Program is "
                    "the server software, to which a client can connect"
                    "in order to change the session and device volumes.</li>"
                    "<li><b>RAAM-AA</b>: RAAM - Android Application is a "
                    "client software, with which the volumes can be remotely "
                    "managed.</li>"
                    "</ul>"
                    "<br><b>Compatibility</b><br>"
                    "For RAAM-WP: Only Windows 10"
                    "<br><br><b>Permissions</b><br>"
                    "RAAM is licensed under LGPL Version 3. Usage, "
                    "modification and distribution are permitted. Anyone can "
                    "contribute to the <a href=\"\">GitHub Project</a> to "
                    "improve this software."));
}
