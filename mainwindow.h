#ifndef __QMAIN_WINDOW__
#define __QMAIN_WINDOW__

#include <QObject>

class QQmlApplicationEngine;
class QKeyEvent;
class VlcInstance;


class MainWindow:  public QObject {
    Q_OBJECT
private:
    QQmlApplicationEngine*  engine;
    QObject*                mainWindow;
    VlcInstance*            vlcInstance;
public:
    explicit MainWindow(QObject* parent = nullptr);
    ~MainWindow();    
protected:
    void keyReleaseEvent(QKeyEvent* event);
};


#endif //__QMAIN_WINDOW__
