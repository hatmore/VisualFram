#include "PythonEngine.h"
#include <fstream>
#include <sstream>

PythonEngine::PythonEngine()
{
}

PythonEngine::~PythonEngine()
{
    //delete pyConfig;
    //pyConfig = nullptr;

}

void PythonEngine::InitailPython(const std::string& path)
{
    //PyConfig* pyConfig = new PyConfig;
    //wchar_t* w_char_path = StringToWchar(path);
    //Py_SetPythonHome(w_char_path);
    //pyConfig->pythonpath_env = w_char_path;
    //pyConfig->home = w_char_path;
    //PyConfig_InitPythonConfig(pyConfig);
    //py::scoped_interpreter guard(pyConfig);
}

int PythonEngine::ExecuteWithParameters(const std::string& filename,  py::dict& global_vars,  py::dict& local_vars)
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

int PythonEngine::ConvertToMap(const py::dict& dict, std::unordered_map<std::string, int> &map_data)
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

wchar_t* PythonEngine::StringToWchar(const std::string& str, UINT codePage)
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