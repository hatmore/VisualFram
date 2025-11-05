#include "FramYoloV11Pose.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramYoloV11Pose window;
    window.show();
    return app.exec();
}
