#include "FramPoseC3D.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramPoseC3D window;
    window.show();
    return app.exec();
}
