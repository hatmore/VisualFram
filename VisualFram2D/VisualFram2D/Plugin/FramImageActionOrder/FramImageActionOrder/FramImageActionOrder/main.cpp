#include "FramImageActionOrder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramImageActionOrder window;
    window.show();
    return app.exec();
}
