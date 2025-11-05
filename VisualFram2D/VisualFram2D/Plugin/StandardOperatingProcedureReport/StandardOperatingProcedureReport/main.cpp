#include "FramStandardOperatingProcedureReport.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FramStandardOperatingProcedureReport window;

    _putenv_s("PYTHONHOME", "D:\\ProgramData\\Anaconda3\\envs\\Python310");
    Py_SetPythonHome(L"D:\\ProgramData\\Anaconda3\\envs\\Python310");
    py::scoped_interpreter guard{};
    py::gil_scoped_release release;


    window.show();
    return app.exec();
}
