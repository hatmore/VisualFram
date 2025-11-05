#include "FramImageMaskFilter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramImageMaskFilter window;
    window.show();
    return app.exec();
}
