#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "PybindEmbeddedModule.h"
#include "VMObject2PyObject.h"
namespace py = pybind11;

class PyBind11PythonEngine
{
public:
    using Ptr = std::shared_ptr<PyBind11PythonEngine>;
    PyBind11PythonEngine();
    ~PyBind11PythonEngine();

    /// <summary>
    /// 初始python运行环境
    /// </summary>
    /// <param name="path"></param>
    void InitailPython(const std::string& path);

    /// <summary>
    /// 预编译python代码
    /// </summary>
    /// <param name="ptr_vmvector"></param>
    /// <returns></returns>
    int PythonPrecompile(const PtrVMVector<PtrVMObject>& ptr_vmvector = nullptr);

    /// <summary>
    /// 预编译python代码
    /// </summary>
    /// <param name="ptr_vmvector"></param>
    /// <returns></returns>
    int PythonDebug(QString & str_debug, const PtrVMVector<PtrVMObject>& ptr_vmvector = nullptr);


    /// <summary>
    ///  执行部分
    /// </summary>
    /// <returns></returns>
    int PythonExecute(const PtrVMVector<PtrVMObject>& ptr, cv::Mat& sop_data);

    /// <summary>
    /// 获得日志信息
    /// </summary>
    /// <param name="str_log"></param>
    const QString &GetPythonLog()  const{
        return this->strLogInfo;
     }

    /// <summary>
    /// 路径 
    /// </summary>
    /// <param name="path"></param>
    void PythonCodeLoad(const std::string& path);
    

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
    /// 执行python文件
    /// </summary>
    /// <param name="filename">文件名</param>
    /// <param name="global_vars">本地参数</param>
    /// <param name="local_vars">Python文件里的参数</param>
    /// <returns></returns>
    int ExecuteWithParameters(const std::string& filename, py::dict& global_vars, py::dict& local_vars);

    /// <summary>
    ///  string转wcahr*
    /// </summary>
    /// <param name="str">输入string</param>
    /// <param name="codePage">编码类型</param>
    /// <returns></returns>
    wchar_t* StringToWchar(const std::string& str, UINT codePage = CP_UTF8);

    /// <summary>
    ///  
    /// </summary>
    /// <param name="pybind11_calss"></param>
    /// <param name="py_dict"></param>
    void VMVector2PyDict(const PtrVMVector<PtrVMObject>& ptr_vmvector, const py::module& pybind11_calss, py::dict& py_dict);

    /// <summary>
    /// VMObject转化为pybind11的dict
    /// </summary>
    /// <param name="ptr_vmvector"></param>
    /// <param name="pybind11_calss"></param>
    /// <param name="py_dict"></param>
    void VMObject2PyDict(const PtrVMObject& ptr_object, const py::module& pybind11_calss, py::dict& py_dict);


    void VMObject2PyList(const PtrVMObject& ptr_object, const py::module& pybind11_calss, py::list& py_list);

private:
    std::string pythonPath;
    std::string envirPath;
    wchar_t* wCharPath{ nullptr };
    PyConfig* pythonConfig{nullptr};
    QString strLogInfo;
    VMObjectStruct2PyObjectClass vmObjectStruct2PyObjectClass;
};

