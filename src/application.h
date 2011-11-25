#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc,argv) {}
    virtual bool notify(QObject *receiver, QEvent *event);
};

#endif // APPLICATION_H
