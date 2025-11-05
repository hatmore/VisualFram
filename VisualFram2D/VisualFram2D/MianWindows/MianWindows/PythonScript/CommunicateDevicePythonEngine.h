#pragma once
#include <string>
#include <windows.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/iostream.h>
namespace py = pybind11;

class CommunicateDevicePythonEngine 
{
public:
    CommunicateDevicePythonEngine();
    ~CommunicateDevicePythonEngine();

    /// <summary>
    /// 初始python运行环境
    /// </summary>
    /// <param name="path"></param>
    void InitailPython(const std::string& path);

    /// <summary>
    /// 加载Pyhton代码地址
    /// </summary>
    /// <param name="path"></param>
    void PythonCodeLoad(const std::string &path);

    /// <summary>
    /// 获得python代码结果
    /// </summary>
    /// <param name="byte_array"></param>
    void GetReviceData(const std::string& byte_array);

    /// <summary>
    /// 获得触发检索类型
    /// </summary>
    /// <param name="type"></param>
    void GetSearchType(std::string& type);

    /// <summary>
    /// 获得执行命令
    /// </summary>
    /// <returns></returns>
    std::unordered_map<std::string, int> GetFlowsExeOrder() { 
        return this->mapFlowNameAndOrder;
    }

private:

    /// <summary>
    /// 执行python文件
    /// </summary>
    /// <param name="filename">文件名</param>
    /// <param name="global_vars">本地参数</param>
    /// <param name="local_vars">Python文件里的参数</param>
    /// <returns></returns>
    int ExecuteWithParameters(const std::string& filename, py::dict& global_vars, py::dict& local_vars);
    /// <summary>
    /// 把dict转化为map，设定死的
    /// </summary>
    /// <param name="dict"></param>
    /// <param name="map_data"></param>
    /// <returns></returns>
    int ConvertToMap(const py::dict& dict, std::unordered_map<std::string, int>& map_data);

    /// <summary>
    ///  string转wcahr*
    /// </summary>
    /// <param name="str">输入string</param>
    /// <param name="codePage">编码类型</param>
    /// <returns></returns>
    wchar_t* StringToWchar(const std::string& str, UINT codePage = CP_UTF8);

private:
    std::string envirPath;
    std::string pythonPath;
    //py::dict *globalVars{nullptr};
    //py::dict *localVars{nullptr};
    PyConfig* pythonConfig{nullptr};
    wchar_t* wCharPath{nullptr};
    std::unordered_map<std::string, int> mapFlowNameAndOrder;
};

