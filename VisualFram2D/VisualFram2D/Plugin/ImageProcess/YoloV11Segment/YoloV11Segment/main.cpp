#include "FramYoloV11Segment.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramYoloV11Segment window;
    window.show();
    return app.exec();
}
