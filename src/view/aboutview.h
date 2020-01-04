#ifndef ABOUT_H
#define ABOUT_H

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include "utils/makros.h"



class AboutView: public QScrollArea {
    Q_OBJECT


public:
    AboutView(QWidget *parent = nullptr);


};

#endif // ABOUT_H
