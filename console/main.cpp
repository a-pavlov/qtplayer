#include <QtCore>
#include <QDebug>

#include "bittorrent/rangememorystorage.h"

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject *parent = 0) : QObject(parent) {}

public slots:
    void run()
    {
        qDebug() << "processing";
        emit finished();
    }

signals:
    void finished();
};

#include "main.moc"

int main(int argc, char *argv[]) {
    qDebug() << "console started";

    QCoreApplication a(argc, argv);

    // Task parented to the application so that it
    // will be deleted by the application.
    Task *task = new Task(&a);

    // This will cause the application to exit when
    // the task signals finished.    
    QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));


    BitTorrent::RangeMemoryStorage rms(libtorrent::file_storage());

    return a.exec();
}
