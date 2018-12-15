#include "mainwindow.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include "Instance.h"
#include "Common.h"
#include "qml/Qml.h"

MainWindow::MainWindow(QObject* parent)
    : QObject(parent)
    , vlcInstance(new VlcInstance(VlcCommon::args(), this)) {
    vlcInstance->setLogLevel(Vlc::DebugLevel);
    engine = new QQmlApplicationEngine(this);    
    engine->load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    Q_ASSERT(engine->rootObjects().size() == 1);
}

MainWindow::~MainWindow() {}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Exit) {
        qDebug() << "exit key pressed";
    }

    event->setAccepted(true);
}
