#include "FramImageSource.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FramImageSource w;
    w.show();
    return a.exec();
}
