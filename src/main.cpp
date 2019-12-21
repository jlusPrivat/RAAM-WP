#include <QApplication>
#include <QTranslator>
#include "utils/makros.h"
#include "view/mainwindow.h"



int main(int argc, char *argv[]) {
    // initialize and localize
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load(QLocale("de"), "raam-wp", "_", ":/translations");
    a.installTranslator(&translator);

    // show and execute
    MainWindow w;
    w.show();
    return a.exec();
}
