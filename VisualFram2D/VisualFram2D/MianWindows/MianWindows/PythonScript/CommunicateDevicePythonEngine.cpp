#include "CommunicateDevicePythonEngine.h"
#include <fstream>
#include <sstream>
#include <chrono>

CommunicateDevicePythonEngine::CommunicateDevicePythonEngine()
{

}

CommunicateDevicePythonEngine::~CommunicateDevicePythonEngine()
{
    delete pythonConfig;
    pythonConfig = nullptr;
}

void CommunicateDevicePythonEngine::InitailPython(const std::string& path)
{
    envirPath = path;
    pythonConfig = new PyConfig;
    pythonConfig->isolated = 1;       // 隔离模式
    pythonConfig->use_environment = 0; // 忽略环境变量
    pythonConfig->install_signal_handlers = 0; // 不安装信号处理程序
    pythonConfig->parse_argv = 0;     // 不解析命令行参数
    wCharPath = StringToWchar(envirPath);
}

void CommunicateDevicePythonEngine::PythonCodeLoad(const std::string& path)
{
    pythonPath = path;
}

void CommunicateDevicePythonEngine::GetReviceData(const std::string& byte_array)
{
    Py_SetPythonHome(wCharPath);
    pythonConfig->pythonpath_env = wCharPath;
    pythonConfig->home = wCharPath;
    //PyConfig_InitPythonConfig(pythonConfig);
    //py::scoped_interpreter guard(pythonConfig);
    py::gil_scoped_acquire acquire;  // 获取GIL

    py::dict global_vars;
    py::dict local_vars;
    int res = ExecuteWithParameters(pythonPath, global_vars, local_vars);
    if (res != 0) {
        return;
    }
    try {
        auto  python_object = local_vars["CommunicateDevice"];
        auto communicate_deviceobject = python_object();
        communicate_deviceobject.attr("SetReviceData")(byte_array);
        py::object result = communicate_deviceobject.attr("GetResult")();
        if (!py::isinstance<py::dict>(result)) {
            return;
        }
        auto dict = result.cast<py::dict>();
        mapFlowNameAndOrder.clear();
        ConvertToMap(dict, mapFlowNameAndOrder);
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python执行错误: " << e.what() << std::endl;
    }
    py::gil_scoped_acquire gil_release;
}

void CommunicateDevicePythonEngine::GetSearchType(std::string& type)
{
    Py_SetPythonHome(wCharPath);
    pythonConfig->pythonpath_env = wCharPath;
    pythonConfig->home = wCharPath;
    //PyConfig_InitPythonConfig(pythonConfig);
    //py::scoped_interpreter guard(pythonConfig);

    py::gil_scoped_acquire acquire;  // 获取GIL

    py::dict global_vars;
    py::dict local_vars;
    int res = ExecuteWithParameters(pythonPath, global_vars, local_vars);
    if (res != 0) {
        return;
    }
    try {
        auto  python_object = local_vars["GetSearchType"];
        auto type_object = python_object().cast<std::string>();
        type = type_object;
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python执行错误: " << e.what() << std::endl;
    }
    py::gil_scoped_acquire gil_release;
}

int CommunicateDevicePythonEngine::ExecuteWithParameters(const std::string& filename, py::dict& global_vars, py::dict& local_vars)
{
    py::scoped_ostream_redirect output;
    py::scoped_estream_redirect error;
    try {
        std::ifstream file(filename);
        std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // 使用自定义命名空间
        py::exec(code, global_vars, local_vars);
        return 0;
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python执行错误: " << e.what() << std::endl;
        return -1;
    }
}

int CommunicateDevicePythonEngine::ConvertToMap(const py::dict& dict, std::unordered_map<std::string, int>& map_data)
{
    if (!py::isinstance<py::dict>(dict)) {
        return -1;
    }
    std::unordered_map<std::string, std::string> result;
    for (auto item : dict) {
        std::string key = item.first.cast<std::string>();
        if (py::isinstance<py::int_>(item.second)) {
            map_data[key] = item.second.cast<int>();
        }
    }
    return 0;
}

wchar_t* CommunicateDevicePythonEngine::StringToWchar(const std::string& str, UINT codePage)
{
    int wlen = MultiByteToWideChar(
        codePage,           // 编码 (CP_UTF8, CP_ACP)
        0,                  // 标志
        str.c_str(),        // 输入字符串
        -1,                 // 自动计算长度
        nullptr,            // 不接收输出
        0                   // 查询大小
    );

    if (wlen == 0) return nullptr;
    // 分配内存
    wchar_t* wstr = new wchar_t[wlen];
    MultiByteToWideChar(codePage, 0, str.c_str(), -1, wstr, wlen);
    return wstr;
}