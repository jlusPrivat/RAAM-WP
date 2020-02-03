#include <QApplication>
#include "controller/maincontroller.h"



int main(int argc, char *argv[]) {
    // initialize
    QApplication a(argc, argv);
    MainController c;
    return a.exec();
}
