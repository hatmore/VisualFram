#include "FramYoloV11Detect.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramYoloV11Detect window;
    window.show();
    return app.exec();
}
