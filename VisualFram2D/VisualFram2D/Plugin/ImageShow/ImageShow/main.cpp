#include "FramImageShow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FramImageShow w;
    w.show();
    return a.exec();
}
