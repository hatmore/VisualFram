#include "FramLogicalOperation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramLogicalOperation window;
    window.show();
    return app.exec();
}
