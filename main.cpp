#include "radarwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RadarWidget w;
//    w.showFullScreen();
    w.show();

    return a.exec();
}
