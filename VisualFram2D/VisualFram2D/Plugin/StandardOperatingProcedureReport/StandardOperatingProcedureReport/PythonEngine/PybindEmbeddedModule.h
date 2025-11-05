#pragma once
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include "../GeneralStruct.h"
namespace py = pybind11;

class  PySOPObject
{
public:
    PySOPObject();
public:
    std::string  toolName{ "" };                //工具名称
    unsigned int nodeID{ 0 };              //所属节点ID
    bool state{ true };              // 节点数据状态
    int calssId{-1};                  //类别信息,可以表达类别
    float confidence{0};            //置信度
    uint64_t timeStamp{0};   //时间戳
};

class  PySOPNodeState :public PySOPObject
{
public:
    PySOPNodeState();

public:
    bool nodeState{false};
};

class  PySOPPoint2d :public PySOPObject
{
public:
    PySOPPoint2d();
public:
    float x{0};
    float y{0};
};

class  PySOPRectangle :public PySOPObject
{
public:
    PySOPRectangle();
public:
    float x{0};
    float y{0};
    int width{ 0 };
    int height{ 0 };
    float angle{ 0 };
};

class PySOPPolygon : public PySOPObject  
{  
public:  
    PySOPPolygon() { ; }  

public:  
    py::list listX;  // Remove template parameter
    py::list listY;  // Remove template parameter  
    py::list listId;  // Remove template parameter  
};

/// <summary>
/// 
/// </summary>
class PySOPList : public PySOPObject
{
public:
    PySOPList() { ; }
public:
    py::list pyList;
};



class PySOPMat : public PySOPObject
{
public:
    PySOPMat() { ; }

public:
    py::array_t<unsigned char> vmMat;
};

