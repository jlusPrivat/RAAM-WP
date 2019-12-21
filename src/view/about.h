#ifndef ABOUT_H
#define ABOUT_H

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include "utils/makros.h"



class About: public QScrollArea {
    Q_OBJECT


public:
    About(QWidget *parent = nullptr);


};

#endif // ABOUT_H
