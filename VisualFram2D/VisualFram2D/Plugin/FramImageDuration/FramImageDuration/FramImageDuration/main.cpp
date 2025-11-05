#include "FramImageDuration.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramImageDuration window;
    window.show();
    return app.exec();
}
