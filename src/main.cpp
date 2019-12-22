#include <QApplication>
#include <QTranslator>
#include "utils/makros.h"
#include "controller/maincontroller.h"



int main(int argc, char *argv[]) {
    // initialize
    QApplication a(argc, argv);
    MainController c;
    return a.exec();
}
