#include "FramImageROISelection.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramImageROISelection window;
    window.show();
    return app.exec();
}
