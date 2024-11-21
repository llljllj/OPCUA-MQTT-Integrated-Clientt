#include "widget.h"
#include "welcome.h"
#include <QApplication>
#include "admin.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    welcome w1;
    w1.show();
    Widget w;
    admin w2;
    QObject::connect(&w1,SIGNAL(showwid()),&w,SLOT(showme()));
    QObject::connect(&w1,SIGNAL(showad()),&w2,SLOT(showmead()));
    QObject::connect(&w2,SIGNAL(showmain()),&w1,SLOT(showme()));
    QObject::connect(&w,SIGNAL(showmain()),&w1,SLOT(showme()));
    return a.exec();
}
