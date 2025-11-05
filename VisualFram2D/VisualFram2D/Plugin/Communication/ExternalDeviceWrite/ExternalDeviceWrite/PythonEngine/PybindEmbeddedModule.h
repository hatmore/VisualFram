#pragma once
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include "../GeneralStruct.h"
namespace py = pybind11;

class  PyVMObject
{
public:
    PyVMObject();
public:
    std::string  toolName{ "" };                //工具名称
    unsigned int nodeID{ 0 };              //所属节点ID
    bool state{ true };              // 节点数据状态
    int calssId{-1};                  //类别信息,可以表达类别
    float confidence{0};            //置信度
    uint64_t timeStamp{0};   //时间戳
};

class  PyVMNodeState :public PyVMObject
{
public:
    PyVMNodeState();

public:
    bool nodeState{false};
};

class  PyVMPoint2d :public PyVMObject
{
public:
    PyVMPoint2d();
public:
    float x{0};
    float y{0};
};

class  PyVMRectangle :public PyVMObject
{
public:
    PyVMRectangle();
public:
    float x{0};
    float y{0};
    int width{ 0 };
    int height{ 0 };
    float angle{ 0 };
};

class PyVMPolygon : public PyVMObject  
{  
public:  
    PyVMPolygon() { ; }  

public:  
    py::list listX;  // Remove template parameter
    py::list listY;  // Remove template parameter  
    py::list listId;  // Remove template parameter  
};

/// <summary>
/// 
/// </summary>
class PyVMList : public PyVMObject
{
public:
    PyVMList() { ; }
public:
    py::list pyList;
};


class PyVMMat : public PyVMObject
{
public:
    PyVMMat() { ; }

public:
    py::array_t<unsigned char> vmMat;
};

