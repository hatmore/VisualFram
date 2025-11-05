#include "FramExternalDeviceRead.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramExternalDeviceRead window;
    window.show();
    return app.exec();
}
