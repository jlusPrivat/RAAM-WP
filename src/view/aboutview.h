#ifndef ABOUT_H
#define ABOUT_H

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include "utils/makros.h"



/**
 * \brief The AboutView class
 *
 * Only for displaying the about tab of this application
 */
class AboutView: public QScrollArea {
    Q_OBJECT


public:
    AboutView(QWidget *parent = nullptr);


};

#endif // ABOUT_H
