#pragma once
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include "PybindEmbeddedModule.h"
#include "../GeneralStruct.h"

class VMObjectStruct2PyObjectClass
{
public:
    VMObjectStruct2PyObjectClass();
    ~VMObjectStruct2PyObjectClass();
    /// <summary>
    /// 获得
    /// </summary>
    /// <param name="ptr"></param>
    void GetVMObject(const  PtrVMVector<PtrVMObject> & ptr);
    
    /// <summary>
    ///  
    /// </summary>
    /// <param name="pybind11_calss"></param>
    /// <param name="py_dict"></param>
    void VMVector2PyDict(const py::module& pybind11_calss, py::dict& py_dict);

private:


    /// <summary>
    /// VMObject转换位Python能识别的形式
    /// </summary>
    /// <param name="ptr_object"></param>
    void VMObject2PyObject(const PtrVMObject &ptr_object);


private:

    PtrVMVector<PtrVMObject> ptrVMVector{nullptr};
};

