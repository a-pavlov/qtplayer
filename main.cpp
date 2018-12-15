#include <QApplication>
#include <QQmlApplicationEngine>
#include "Qml.h"
#include "mainwindow.h"

int main(int argc, char** argv) {
    //qunsetenv("VLC_PLUGIN_PATH");
    QApplication app(argc, argv);
    VlcQml::registerTypes();
    MainWindow mw;
    return app.exec();
}
