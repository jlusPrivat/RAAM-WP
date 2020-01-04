#include "aboutview.h"



AboutView::AboutView (QWidget *parent)
    : QScrollArea(parent) {
    setBackgroundRole(QPalette::Light);
    setFrameShape(QFrame::NoFrame);
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
    image->setFixedWidth(pm.width() + (2 * image->margin()));

    QLabel *lbl = new QLabel(this);
    layout->addWidget(lbl);
    lbl->setWordWrap(true);
    lbl->setTextFormat(Qt::RichText);
    lbl->setOpenExternalLinks(true);
    lbl->setText(tr("<b>Remote Application Audio Program</b> "
                    "(RAAM-WP), developed by <a href=\"https://jlus.de\">"
                    "Jonathan Lusky</a> as <a href=\"https://example.org\">"
                    "bachelor's thesis</a>."
                    "<br><br><b>Version</b><br>%1"
                    "<br><br><b>Permissions</b><br>"
                    "RAAM is licensed under LGPL Version 3. Usage, "
                    "modification and distribution are permitted. Anyone can "
                    "contribute to the "
                    "<a href=\"https://github.com/jlusPrivat/RAAM-WP\">"
                    "GitHub Project</a> to improve this software.").arg(VERSION));
}
