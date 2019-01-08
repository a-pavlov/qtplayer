#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>

class Application : public QCoreApplication {
public:
    Application(int &argc, char **argv);
    ~Application();
    int exec();
};

#endif // APPLICATION_H
