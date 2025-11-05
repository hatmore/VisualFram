#include "MianWindows.h"
#include "GoodMainWindow.h"
#include <QScreen>
#include <QtWidgets/QApplication>
#include <Windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MianWindows w;
    // 设置 PYTHONHOME（必须在解释器初始化前设置）
    _putenv_s("PYTHONHOME", "D:\\VisualFram2.0II\\VisualFram2D\\VisualFram2D\\ThirdParty\\Python310");
    Py_SetPythonHome(L"D:\\VisualFram2.0II\\VisualFram2D\\VisualFram2D\\ThirdParty\\Python310");
    py::scoped_interpreter guard{};
    py::gil_scoped_release release;

    w.show();
    return a.exec();
}
