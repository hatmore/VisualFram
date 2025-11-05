#pragma once
#include <string>
#include <windows.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

class PythonEngine
{
public:
    PythonEngine();
    ~PythonEngine();

    /// <summary>
    /// 初始python运行环境
    /// </summary>
    /// <param name="path"></param>
    void InitailPython(const std::string &path);

    /// <summary>
    /// 执行python文件
    /// </summary>
    /// <param name="filename">文件名</param>
    /// <param name="global_vars">本地参数</param>
    /// <param name="local_vars">Python文件里的参数</param>
    /// <returns></returns>
    int ExecuteWithParameters(const std::string& filename,  py::dict& global_vars,  py::dict& local_vars);



protected:
    /// <summary>
    /// 把dict转化为map，设定死的
    /// </summary>
    /// <param name="dict"></param>
    /// <param name="map_data"></param>
    /// <returns></returns>
    int ConvertToMap(const py::dict& dict, std::unordered_map<std::string, int>& map_data);


private:
    /// <summary>
    ///  string转wcahr*
    /// </summary>
    /// <param name="str">输入string</param>
    /// <param name="codePage">编码类型</param>
    /// <returns></returns>
    wchar_t* StringToWchar(const std::string& str, UINT codePage = CP_UTF8);

private:
    std::string pythonPath;
    //PyConfig* pyConfig{nullptr};
};

