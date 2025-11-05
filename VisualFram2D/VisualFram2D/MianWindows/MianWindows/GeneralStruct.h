#pragma once
#include <QObject>
#include <QString>
#include <vector>
#include <mutex>
#include <QImage>
#include <iostream>
#include <QtNodes/NodeData.hpp>
#include <opencv2\opencv.hpp>

/// <summary>
/// 读取插件的元数据
/// </summary>
struct PluginsMetaData
{
	QString author;  //作者
	QString date;    //日期
	QString name;    //插件名称
	QString	classes;   //所属类别
	QString version;    //版本号
	QString	dependencies;  //依赖关系
};

//日志状态
enum LOGTYPE {
	DEBUG = 0,
	INFO = 1,
	WARN = 2,
	ERRORS = 3,
	FATAL = 4
};

//插件运行状态
enum RUNNINGSTATE {
	RUNNING_SUCESS = 0,  //运行成功
	RUNNING_FAIL = 1, //运行失败，节点本身运行失败
	DATA_NULL = 2,   //输入数据是空
	PARAM_EINVAL = 3,         //输入参数错误，参数不合法
};


enum VMDATATYPE {
    VM_NULL = -1,
    VM_BOOL = 0,
    VM_POINT = 1,
    VM_RECTANGLE = 2,
    VM_POLYGON = 3,
    VM_MAT = 5,
    VM_VECTOR = 100,
    VM_UNORDERED_MAP = 101,
};

/// <summary>
/// 数据流基础类型
/// </summary>
class VMObject : public QObject
{
public:
    using Ptr = std::shared_ptr<VMObject>;
	VMObject() :nodeDataType(QtNodes::NodeDataType{ "-1","NULL" }), cgType(VMDATATYPE::VM_NULL) { ; }
	VMObject(const QtNodes::NodeDataType node_type, const VMDATATYPE& _type_name) :nodeDataType(node_type), cgType(_type_name) { ; }
    VMObject(const VMObject& object) :nodeDataType(QtNodes::NodeDataType{ "-1","NULL" }), cgType(VMDATATYPE::VM_NULL){
        this->toolName = object.toolName;
        this->nodeID = object.nodeID;
        this->state = object.state;
        this->calssId = object.calssId;
        this->confidence = object.confidence;
        this->timeStamp = object.timeStamp;
    }

	VMObject& operator = (const VMObject& object) {
		if (this != &object) {
			this->toolName = object.toolName;
			this->nodeID = object.nodeID;
			this->state = object.state;
            this->calssId = object.calssId;
            this->confidence = object.confidence;
            this->timeStamp = object.timeStamp;
		}
		return *this;
	}

public:
	QString  toolName{ "" };                //工具名称
	unsigned int nodeID{ 0 };              //所属节点ID
	bool state{ true };              // 节点数据状态
    int calssId;                  //类别信息,可以表达类别
    float confidence;            //置信度
    uint64_t timeStamp;   //时间戳
    std::mutex mtx;
	const VMDATATYPE cgType;                //类型名称，为了寻找对应的变量
	const QtNodes::NodeDataType nodeDataType;  //数据类型
};
using PtrVMObject = std::shared_ptr<VMObject>;

class VMNodeState :public VMObject {
public:
    using Ptr = std::shared_ptr<VMNodeState>;
	VMNodeState() :VMObject(QtNodes::NodeDataType{ "0","Bool" }, VMDATATYPE::VM_BOOL) { ; }
    VMNodeState(const VMNodeState& state) :VMObject(QtNodes::NodeDataType{ "0","Bool" }, VMDATATYPE::VM_BOOL) {
        this->nodeState = state.nodeState;
    }
    VMNodeState(const bool &state) :VMObject(QtNodes::NodeDataType{ "0","Bool" }, VMDATATYPE::VM_BOOL) , nodeState(state){ ; }
    VMNodeState& operator = (const VMNodeState& state) {
        if (this != &state) {
            this->nodeState = state.nodeState;
        }
        return *this;
    }
public:
	bool nodeState;
};
using PtrVMNodeState = std::shared_ptr<VMNodeState>;


class VMPoint2d :public VMObject
{
public:
    using Ptr = std::shared_ptr<VMPoint2d>;
	VMPoint2d() :VMObject(QtNodes::NodeDataType{ "1","Point" }, VMDATATYPE::VM_POINT) { ; }
	VMPoint2d(const VMPoint2d& p) :VMObject(QtNodes::NodeDataType{ "1","Point" }, VMDATATYPE::VM_POINT) {
		this->x = p.x;
		this->y = p.y;
	}
	VMPoint2d(float _x, float _y) :VMObject(QtNodes::NodeDataType{ "1","Point" }, VMDATATYPE::VM_POINT), x(_x), y(_y) { ; }
	VMPoint2d& operator = (const VMPoint2d& p) {
		if (this != &p) {
			this->x = p.x;
			this->y = p.y;
		}
		return *this;
	}

public:
    float x;
    float y;
};
using PtrVMPoint2d = std::shared_ptr<VMPoint2d>;

/// <summary>
/// 矩形
/// </summary>
class VMRectangle :public VMObject
{
public:
    using Ptr = std::shared_ptr<VMRectangle>;
	VMRectangle() :VMObject(QtNodes::NodeDataType{ "2","Rectangle" }, VMDATATYPE::VM_RECTANGLE) { ; }
	VMRectangle(const VMRectangle& rectangle) :VMObject(QtNodes::NodeDataType{ "2","Rectangle" }, VMDATATYPE::VM_RECTANGLE) {
        this->width = rectangle.width;
        this->height = rectangle.height;
        this->centerPoint = rectangle.centerPoint;
        this->angle = rectangle.angle;

	}
    VMRectangle(const int &w, const int &h, const VMPoint2d &p, const float &angl) :VMObject(QtNodes::NodeDataType{ "2","Rectangle" }, VMDATATYPE::VM_RECTANGLE) , \
        width(w), height(h), centerPoint(p), angle(angl) { ; }
    VMRectangle& operator = (const VMRectangle& rectangle) {
        if (this != &rectangle) {
            this->width = rectangle.width;
            this->height = rectangle.height;
            this->centerPoint = rectangle.centerPoint;
            this->angle = rectangle.angle;
        }
        return *this;
    }

public:
    int width{0};
    int height{0};
    VMPoint2d centerPoint{0,0};
    float angle{0};
};
using PtrVMRectangle = std::shared_ptr<VMRectangle>;


class VMPolygon :public VMObject {
public:
    using Ptr = std::shared_ptr<VMPolygon>;
    VMPolygon() :VMObject(QtNodes::NodeDataType{ "3","Polygon" }, VMDATATYPE::VM_POLYGON) { ; }
    VMPolygon(const  std::vector<VMPoint2d> points) :VMObject(QtNodes::NodeDataType{ "3","Polygon" }, VMDATATYPE::VM_POLYGON) {
        this->vectorPoints = points;
    }
    VMPolygon(const VMPolygon& polygon) :VMObject(QtNodes::NodeDataType{ "3","Polygon" }, VMDATATYPE::VM_POLYGON) {
        this->vectorPoints = polygon.vectorPoints;
    }
    VMPolygon& operator = (const VMPolygon& polygon) {
        if (this != &polygon) {
            this->vectorPoints = polygon.vectorPoints;
        }
        return *this;
    }

public:
    std::vector<VMPoint2d> vectorPoints;
};

/// <summary>
/// 输出Mat图像
/// </summary>
class VMMat :public VMObject
{
public:
    using Ptr = std::shared_ptr<VMMat>;
	VMMat() :VMObject(QtNodes::NodeDataType{ "5","Mat" }, VMDATATYPE::VM_MAT) { ; }
    VMMat(const VMMat& mat) :VMObject(QtNodes::NodeDataType{ "5","Mat" }, VMDATATYPE::VM_MAT) { this->vmMat = mat.vmMat; }
    VMMat(const cv::Mat& mat) :VMObject(QtNodes::NodeDataType{ "5","Mat" }, VMDATATYPE::VM_MAT) { this->vmMat = mat; }
    VMMat& operator = (const VMMat& mat) {
        if (this != &mat) {
            this->vmMat = mat.vmMat;
        }
        return *this;
    }
    VMMat& Clone() {
        VMMat vm_mat;
        vm_mat.vmMat = this->vmMat.clone();
        return vm_mat;
    }
public:
	cv::Mat vmMat;  //输出图像
};
using PtrVMMat = std::shared_ptr<VMMat>;

/// <summary>
/// 多个目标使用Vector
/// </summary>
/// <typeparam name="T">模板，可以是点、矩形、圆等等</typeparam>
template <class T>
class VMVector :public VMObject
{
public:
    using Ptr = std::shared_ptr<VMVector<T>>;
	VMVector() :VMObject(QtNodes::NodeDataType{ "100","Vector" }, VMDATATYPE::VM_VECTOR) { ; }
    VMVector(const VMVector& vector_) :VMObject(QtNodes::NodeDataType{ "100","Vector" }, VMDATATYPE::VM_VECTOR) { this->vmVector = vector_.vmVector; }
    VMVector(const std::vector<T> & vector_) :VMObject(QtNodes::NodeDataType{ "100","Vector" }, VMDATATYPE::VM_VECTOR) { this->vmVector =vector_; }
    VMVector& operator = (const  VMVector& vector_) {
        if (this != &vector_) {
            this->vmVector = vector_.vmVector;
        }
        return *this;
    }

public:
	std::vector<T> vmVector;
};
template <class T>
using PtrVMVector = std::shared_ptr<VMVector<T>>;

/// <summary>
/// VM_UNORDERED_MAP
/// </summary>
template <class T1, class T2>
class VMUnorderedMap :public VMObject
{
public:
    using Ptr = std::shared_ptr<VMUnorderedMap>;
    VMUnorderedMap() :VMObject(QtNodes::NodeDataType{ "101","Unordered_map" }, VMDATATYPE::VM_UNORDERED_MAP) { ; }
    VMUnorderedMap(const VMUnorderedMap& map) :VMObject(QtNodes::NodeDataType{ "101","Unordered_map" }, VMDATATYPE::VM_UNORDERED_MAP) { this->vmMap = map.vmMap; }
    VMUnorderedMap(const std::unordered_map<T1, T2>& map) :VMObject(QtNodes::NodeDataType{ "101","Unordered_map" }, VMDATATYPE::VM_UNORDERED_MAP) { this->vmMap = map; }
    VMUnorderedMap& operator = (const VMUnorderedMap& map) {
        if (this != &map) {
            this->vmMap = map.vmMap;
        }
        return *this;
    }

public:
    std::unordered_map<T1, T2> vmMap; // 存储键值对数据
};
template <class T1, class T2>
using PtrVMUnorderedMap = std::shared_ptr<VMUnorderedMap<T1, T2>>;

/// <summary>
/// 外部数据类型
/// </summary>
enum EXDEVICEDTATTYPE {
	EXCAMERA2D = 0,
	EXCAMERA3D = 1,
	EXINT = 2,
	EXFLOAT = 3,
	EXSTRING = 4
};

//外部设备数据类型，主要是相机设备
class ExDeviceData : public VMObject {
public:
	using Ptr = std::shared_ptr<ExDeviceData>;
	ExDeviceData() :exDeviceDataType(EXDEVICEDTATTYPE::EXCAMERA2D) { ; }
	ExDeviceData(const EXDEVICEDTATTYPE& type) :exDeviceDataType(type) { ; }

	EXDEVICEDTATTYPE exDeviceDataType{ EXDEVICEDTATTYPE::EXCAMERA2D };
};
using PtrExDeviceData = std::shared_ptr<ExDeviceData>;

/// <summary>
/// 2d数据
/// </summary>
class DeviceCameraData2D :public ExDeviceData {
public:
	using Ptr = std::shared_ptr<DeviceCameraData2D>;
    DeviceCameraData2D() :ExDeviceData(EXDEVICEDTATTYPE::EXCAMERA2D) { ; }	
public:
    cv::Mat cameraData;
};
using PtrDeviceCameraData2D = std::shared_ptr<DeviceCameraData2D>;

/// <summary>
/// 通信数据
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class DeviceCommunicationData :public ExDeviceData {
public:
	using Ptr = std::shared_ptr<DeviceCommunicationData<T>>;
	T receiveData;
};
template <class T>
using PtrDeviceCommunicationData = std::shared_ptr<DeviceCommunicationData<T>>;

