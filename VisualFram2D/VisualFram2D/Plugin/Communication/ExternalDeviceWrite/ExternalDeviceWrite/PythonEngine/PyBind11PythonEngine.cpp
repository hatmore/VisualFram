#include "PyBind11PythonEngine.h"
#include <Python.h>
#include <pybind11/pybind11.h> 
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/iostream.h>


namespace py = pybind11;
using namespace std;


float PI = 3.14;
int VERSION = 1;

std::string ListStr(const py::list& py_list) {
    return py::str(py_list).cast<std::string>();
}

int GetArraySize(const py::array_t<unsigned char>& array, const int idex = 0) {
    if (array.ndim() != 2)
        throw std::runtime_error("1-channel image must be 2 dims ");
    py::buffer_info buf = array.request();
    return buf.shape[idex];
}

// 定义Python类
PYBIND11_EMBEDDED_MODULE(Pybind11Class, m) {
    // 基类
    py::class_<PyVMObject>(m, "PyVMObject")
        .def(py::init<>(), "Init")
        .def_readwrite("toolName", &PyVMObject::toolName)
        .def_readwrite("nodeID", &PyVMObject::nodeID)
        .def_readwrite("state", &PyVMObject::state)
        .def_readwrite("calssId", &PyVMObject::calssId)
        .def_readwrite("confidence", &PyVMObject::confidence)
        .def_readwrite("timeStamp", &PyVMObject::timeStamp)
        .def("__repr__", [](const PyVMObject& object) {
        return "<PyVMObject = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + "; timeStamp:" + std::to_string(object.timeStamp) + ">";
            });

    py::class_<PyVMNodeState, PyVMObject>(m, "PyVMNodeState")
        .def(py::init<>(), "Init")
        .def_readwrite("nodeState", &PyVMNodeState::nodeState)
        .def("__repr__", [](const PyVMNodeState& object) {
        return "<PyVMNodeState = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + "; timeStamp:" + std::to_string(object.timeStamp) + " nodeState:" + std::to_string(object.nodeState) + ">";
            });

    py::class_<PyVMPoint2d, PyVMObject>(m, "PyVMPoint2d")
        .def(py::init<>(), "Init")
        .def_readwrite("x", &PyVMPoint2d::x)
        .def_readwrite("y", &PyVMPoint2d::y)
        .def("__repr__", [](const PyVMPoint2d& object) {
        return "<PyVMPoint2d = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + "; timeStamp:" + std::to_string(object.timeStamp) + " x:" + std::to_string(object.x) + " y:" + std::to_string(object.x) + ">";
            });

    py::class_<PyVMRectangle, PyVMObject>(m, "PyVMRectangle")
        .def(py::init<>(), "Init")
        .def_readwrite("width", &PyVMRectangle::width)
        .def_readwrite("height", &PyVMRectangle::height)
        .def_readwrite("x", &PyVMRectangle::x)
        .def_readwrite("y", &PyVMRectangle::y)
        .def_readwrite("angle", &PyVMRectangle::angle)
        .def("__repr__", [](const PyVMRectangle& object) {
        return "<PyVMRectangle = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + "; timeStamp:" + std::to_string(object.timeStamp)
            + " width:" + std::to_string(object.width) + "; height:" + std::to_string(object.height) + \
            "; x:" + std::to_string(object.x) + "; y:" + std::to_string(object.y) + "; angle:" + std::to_string(object.angle) + ">";
            });

    py::class_<PyVMPolygon, PyVMObject>(m, "PyVMPolygon")
        .def(py::init<>(), "Init")
        .def_readwrite("listX", &PyVMPolygon::listX)
        .def_readwrite("listY", &PyVMPolygon::listY)
        .def_readwrite("listId", &PyVMPolygon::listId)
        .def("__repr__", [](const PyVMPolygon& object) {
        return "<PyVMRectangle = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) +
            " listX:" + ListStr(object.listX) + " listY:" + ListStr(object.listY) + " listId:" + ListStr(object.listId) + ">";
            });


    py::class_<PyVMList, PyVMObject>(m, "PyVMList")
        .def(py::init<>(), "Init")
        .def_readwrite("pyList", &PyVMList::pyList)
        .def("__repr__", [](const PyVMList& object) {
        return "<PyVMList = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + " pyList:" + ListStr(object.pyList) + ">";
            });

    py::class_<PyVMMat, PyVMObject>(m, "PyVMMat")
        .def(py::init<>(), "Init")
        .def_readwrite("vmMat", &PyVMMat::vmMat)
        .def("__repr__", [](const PyVMMat& object) {
        return "<PyVMRectangle = toolName:'" + object.toolName + "; nodeID:" + std::to_string(object.nodeID) + \
            "; state:" + std::to_string(object.state) + "; calssId:" + std::to_string(object.calssId)   \
            + "; confidence:" + std::to_string(object.confidence) + " imagWidth:" + std::to_string(GetArraySize(object.vmMat, 0)) + \
            " imageHeight:" + std::to_string(GetArraySize(object.vmMat, 0)) + ">";
            });
}


PyBind11PythonEngine::PyBind11PythonEngine()
{
}

PyBind11PythonEngine::~PyBind11PythonEngine()
{
    if (pythonConfig != nullptr) {
        delete pythonConfig;
        pythonConfig = nullptr;
    }
}

void PyBind11PythonEngine::InitailPython(const std::string& path)
{
    envirPath = path;
    pythonConfig = new PyConfig;

    pythonConfig->isolated = 1;       // 隔离模式
    pythonConfig->use_environment = 0; // 忽略环境变量
    pythonConfig->install_signal_handlers = 0; // 不安装信号处理程序
    pythonConfig->parse_argv = 0;     // 不解析命令行参数

    wCharPath = StringToWchar(envirPath);
}

void PyBind11PythonEngine::PythonCodeLoad(const std::string& path)
{
    pythonPath = path;
}

int PyBind11PythonEngine::PythonPrecompile(const PtrVMVector<PtrVMObject>& ptr_vmvector)
{
    //  Py_SetPythonHome(wCharPath);
    pythonConfig->pythonpath_env = wCharPath;
    pythonConfig->home = wCharPath;
    //PyConfig_InitPythonConfig(pythonConfig);
    //Py_InitializeFromConfig(pythonConfig);
    //py::scoped_interpreter guard{ pythonConfig };
    py::gil_scoped_acquire acquire;  // 获取GIL
    py::dict global_vars;
    py::dict local_vars;
    py::module sys = py::module::import("sys");
    py::module pybind11_calss = py::module::import("Pybind11Class");

    try {
        std::ifstream file(pythonPath);
        std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        py::object scope = py::module_::import("__main__").attr("__dict__");
        py::exec(code, scope);
        //   Py_Finalize();
         //  PyConfig_Clear(pythonConfig);  // 释放资源
        py::gil_scoped_release release;
        return 0;
    }
    catch (const py::error_already_set& e) {
        py::gil_scoped_release release;
        return -1;
    }
    return 0;
}

int PyBind11PythonEngine::PythonDebug(QString& str_debug, const PtrVMVector<PtrVMObject>& ptr_vmvector)
{
    pythonConfig->pythonpath_env = wCharPath;
    pythonConfig->home = wCharPath;
    py::gil_scoped_acquire acquire;  // 获取GIL
    py::dict global_vars;
    py::dict local_vars;
    py::module sys = py::module::import("sys");
    py::module numpy = py::module::import("numpy");
    py::module pybind11_calss = py::module::import("Pybind11Class");
    py::dict py_dict;

    VMVector2PyDict(ptr_vmvector, pybind11_calss, py_dict);
    global_vars["VMOjectClass"] = py_dict;
    global_vars["numpy"] = numpy;
    global_vars["Pybind11Class"] = pybind11_calss;
    global_vars["VMOjectClass"] = py_dict;
    int res = ExecuteWithParameters(pythonPath, global_vars, local_vars);
    if (res != 0) {
        py::gil_scoped_release release;
        return -1;
    }
    try {
        auto  python_object = local_vars["CommunicateDeviceWrite"];
        auto deviceobject = python_object();
        py::object info = deviceobject.attr("GetVMObjectInfo")();
        py::object log_info = deviceobject.attr("GetLogInfo")();

        if (!py::isinstance<py::str>(info) || !py::isinstance<py::str>(log_info)) {
            return -1;
        }
        std::string str_info = info.cast<py::str>();
        str_debug = QString::fromStdString(str_info);
        std::string str_log_info = log_info.cast<py::str>();
        this->strLogInfo = QString::fromStdString(str_log_info);
        py::gil_scoped_release release;
    }
    catch (const py::error_already_set& e) {
        py::gil_scoped_release release;
        return -1;
    }
    return 0;
}

int PyBind11PythonEngine::PythonExecute(const PtrVMVector<PtrVMObject>& ptr_vmvector, QByteArray& byte_array)
{
    byte_array.clear();
    pythonConfig->pythonpath_env = wCharPath;
    pythonConfig->home = wCharPath;
    py::gil_scoped_acquire acquire;  // 获取GIL
    py::dict global_vars;
    py::dict local_vars;
    py::module sys = py::module::import("sys");
    py::module numpy = py::module::import("numpy");
    py::module pybind11_calss = py::module::import("Pybind11Class");
    py::dict py_dict;
    VMVector2PyDict(ptr_vmvector, pybind11_calss, py_dict);
    int sieze = py_dict.size();

    global_vars["sys"] = sys;
    global_vars["numpy"] = numpy;
    global_vars["Pybind11Class"] = pybind11_calss;
    global_vars["VMOjectClass"] = py_dict;
    int res = ExecuteWithParameters(pythonPath, global_vars, local_vars);
    if (res != 0) {
        py::gil_scoped_release release;
        return -1;
    }
    try {
        auto  python_object = local_vars["CommunicateDeviceWrite"];
        auto deviceobject = python_object();
        py::object run = deviceobject.attr("Run")();
        py::object info = deviceobject.attr("GetWriteData")();
        py::object log_info = deviceobject.attr("GetLogInfo")();
        if (!py::isinstance<py::str>(info) || !py::isinstance<py::str>(log_info)) {
            return -1;
        }
        std::string str_info = info.cast<py::str>();
        byte_array = QByteArray::fromStdString(str_info);
        std::string str_log_info = log_info.cast<py::str>();
        this->strLogInfo = QString::fromStdString(str_log_info);
        py::gil_scoped_release release;
    }
    catch (const py::error_already_set& e) {
        py::gil_scoped_release release;
        return -1;
    }
    return 0;
}

void PyBind11PythonEngine::VMVector2PyDict(const PtrVMVector<PtrVMObject>& ptr_vmvector, const py::module& pybind11_calss, py::dict& py_dict)
{
    for (size_t i = 0; i < ptr_vmvector->vmVector.size(); i++) {
        PtrVMObject ptr_object = ptr_vmvector->vmVector[i];
        VMObject2PyDict(ptr_object, pybind11_calss, py_dict);
    }
}

void PyBind11PythonEngine::VMObject2PyDict(const PtrVMObject& ptr_object, const py::module& pybind11_calss, py::dict& py_dict)
{
    unsigned int node_id = ptr_object->nodeID;
    auto str_node_id = std::to_string(node_id);
    VMDATATYPE cg_type = ptr_object->cgType;

    switch (cg_type) {
    case VM_NULL:
        break;
    case VM_BOOL: {
        auto vm_p = std::static_pointer_cast<VMNodeState>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMNodeState");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("nodeState") = vm_p->nodeState;
        py_dict[str_node_id.c_str()] = node_state;
    }
                break;
    case VM_POINT: {
        auto vm_p = std::static_pointer_cast<VMPoint2d>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMPoint2d");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("x") = vm_p->x;
        node_state.attr("y") = vm_p->y;
        py_dict[str_node_id.c_str()] = node_state;
    }
                 break;
    case VM_RECTANGLE: {
        auto vm_p = std::static_pointer_cast<VMRectangle>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMRectangle");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("width") = vm_p->width;
        node_state.attr("height") = vm_p->height;
        node_state.attr("x") = vm_p->centerPoint.x;
        node_state.attr("y") = vm_p->centerPoint.y;
        node_state.attr("angle") = vm_p->angle;
        py_dict[str_node_id.c_str()] = node_state;
    }
                     break;
    case VM_POLYGON: {
        auto vm_p = std::static_pointer_cast<VMPolygon>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMPolygon");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        PyVMPolygon py_polygon;
        for (size_t i = 0; i < vm_p->vectorPoints.size(); i++) {
            VMPoint2d vm_point = vm_p->vectorPoints[i];
            py_polygon.listX.append(vm_point.x);
            py_polygon.listY.append(vm_point.y);
            py_polygon.listId.append(vm_point.nodeID);
        }
        node_state.attr("listX") = py_polygon.listX;
        node_state.attr("listY") = py_polygon.listY;
        node_state.attr("listId") = py_polygon.listId;
        py_dict[str_node_id.c_str()] = node_state;
    }
                   break;
    case VM_MAT: {
        auto vm_p = std::static_pointer_cast<VMMat>(ptr_object);
        cv::Mat input = vm_p->vmMat.clone();
        //// 将cv::Mat转换为py::array_t<unsigned char>，暂时假设是3通道图像，只支持BGR格式
        py::array_t<unsigned char> dst = py::array_t<unsigned char>({ input.rows,input.cols,3 }, input.data);
        py::object py_node_state = pybind11_calss.attr("PyVMMat");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("vmMat") = dst;
        py_dict[str_node_id.c_str()] = node_state;
    }
               break;
    case VM_VECTOR: {
        auto ptr_vmvector = std::static_pointer_cast<VMVector<PtrVMObject>>(ptr_object);
        py::object py_vmlist = pybind11_calss.attr("PyVMList");
        py_vmlist.attr("toolName") = ptr_vmvector->toolName.toStdString();
        py_vmlist.attr("nodeID") = ptr_vmvector->nodeID;
        py_vmlist.attr("state") = ptr_vmvector->state;
        py_vmlist.attr("calssId") = ptr_vmvector->calssId;
        py_vmlist.attr("confidence") = ptr_vmvector->confidence;
        py_vmlist.attr("timeStamp") = ptr_vmvector->timeStamp;
        py::list py_vector;
        for (size_t i = 0; i < ptr_vmvector->vmVector.size(); i++) {
            PtrVMObject ptr_object = ptr_vmvector->vmVector[i];
            VMObject2PyList(ptr_object, pybind11_calss, py_vector);
        }
        py_vmlist.attr("pyList") = py_vector;
        py_dict[str_node_id.c_str()] = py_vmlist;
    }
                  break;
    case VM_UNORDERED_MAP: {
        auto ptr_vmvmap = std::static_pointer_cast<VMUnorderedMap<int, PtrVMObject>>(ptr_object);

        py::object py_vmlist = pybind11_calss.attr("PyVMList");
        py_vmlist.attr("toolName") = ptr_vmvmap->toolName.toStdString();
        py_vmlist.attr("nodeID") = ptr_vmvmap->nodeID;
        py_vmlist.attr("state") = ptr_vmvmap->state;
        py_vmlist.attr("calssId") = ptr_vmvmap->calssId;
        py_vmlist.attr("confidence") = ptr_vmvmap->confidence;
        py_vmlist.attr("timeStamp") = ptr_vmvmap->timeStamp;
        py::list py_map;
        auto vm_map = ptr_vmvmap->vmMap;
        for (auto it = vm_map.begin(); it != vm_map.end(); ++it) {
            int key = it->first;
            PtrVMObject ptr_object = it->second;
            VMObject2PyList(ptr_object, pybind11_calss, py_map);
        }
        py_vmlist.attr("pyList") = py_map;
        py_dict[str_node_id.c_str()] = py_vmlist;
    }
                  break;
    default:
        break;
    }
}

void PyBind11PythonEngine::VMObject2PyList(const PtrVMObject& ptr_object, const py::module& pybind11_calss, py::list& py_list)
{
    if (ptr_object == nullptr) {
        return;
    }
    unsigned int node_id = ptr_object->nodeID;
    auto str_node_id = std::to_string(node_id);
    VMDATATYPE cg_type = ptr_object->cgType;

    switch (cg_type) {
    case VM_NULL:
        break;
    case VM_BOOL: {
        auto vm_p = std::static_pointer_cast<VMNodeState>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMNodeState");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("nodeState") = vm_p->nodeState;
        py_list.append(node_state);
    }
                break;
    case VM_POINT: {
        auto vm_p = std::static_pointer_cast<VMPoint2d>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMPoint2d");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("x") = vm_p->x;
        node_state.attr("y") = vm_p->y;
        py_list.append(node_state);
    }
                 break;
    case VM_RECTANGLE: {
        auto vm_p = std::static_pointer_cast<VMRectangle>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMRectangle");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("width") = vm_p->width;
        node_state.attr("height") = vm_p->height;
        node_state.attr("x") = vm_p->centerPoint.x;
        node_state.attr("y") = vm_p->centerPoint.y;
        node_state.attr("angle") = vm_p->angle;
        py_list.append(node_state);
    }
                     break;
    case VM_POLYGON: {
        auto vm_p = std::static_pointer_cast<VMPolygon>(ptr_object);
        py::object py_node_state = pybind11_calss.attr("PyVMPolygon");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        PyVMPolygon py_polygon;
        for (size_t i = 0; i < vm_p->vectorPoints.size(); i++) {
            VMPoint2d vm_point = vm_p->vectorPoints[i];
            py_polygon.listX.append(vm_point.x);
            py_polygon.listY.append(vm_point.y);
            py_polygon.listId.append(vm_point.nodeID);
        }
        node_state.attr("listX") = py_polygon.listX;
        node_state.attr("listY") = py_polygon.listY;
        node_state.attr("listId") = py_polygon.listId;
        py_list.append(node_state);
    }
                   break;
    case VM_MAT: {
        auto vm_p = std::static_pointer_cast<VMMat>(ptr_object);
        cv::Mat input = vm_p->vmMat.clone();
        //// 将cv::Mat转换为py::array_t<unsigned char>，暂时假设是3通道图像，只支持BGR格式
        py::array_t<unsigned char> dst = py::array_t<unsigned char>({ input.rows,input.cols,3 }, input.data);
        py::object py_node_state = pybind11_calss.attr("PyVMMat");
        py::object node_state = py_node_state();
        node_state.attr("toolName") = vm_p->toolName.toStdString();
        node_state.attr("nodeID") = vm_p->nodeID;
        node_state.attr("state") = vm_p->state;
        node_state.attr("calssId") = vm_p->calssId;
        node_state.attr("confidence") = vm_p->confidence;
        node_state.attr("timeStamp") = vm_p->timeStamp;
        node_state.attr("vmMat") = dst;
        py_list.append(node_state);
    }
               break;
    case VM_VECTOR: {
        auto ptr_vmvector = std::static_pointer_cast<VMVector<PtrVMObject>>(ptr_object);
        py::list py_vector;
        for (size_t i = 0; i < ptr_vmvector->vmVector.size(); i++) {
            PtrVMObject ptr_object = ptr_vmvector->vmVector[i];
            VMObject2PyList(ptr_object, pybind11_calss, py_vector);
        }
        py_list.append(py_vector);
    }
                  break;
    case VM_UNORDERED_MAP: {
        auto ptr_vmvmap = std::static_pointer_cast<VMUnorderedMap<int, PtrVMObject>>(ptr_object);
        py::list py_map;
        auto vm_map = ptr_vmvmap->vmMap;
        for (auto it = vm_map.begin(); it != vm_map.end(); ++it) {
            int key = it->first;
            PtrVMObject ptr_object = it->second;
            VMObject2PyList(ptr_object, pybind11_calss, py_map);
        }
        py_list.append(py_map);
    }
                         break;
    default:
        break;
    }
}

int PyBind11PythonEngine::ExecuteWithParameters(const std::string& filename, py::dict& global_vars, py::dict& local_vars)
{
    //py::scoped_ostream_redirect output;
    //py::scoped_estream_redirect error;
    try {
        std::ifstream file(filename);
        if (!file.good()) {
            return -1;
        }
        std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // 使用自定义命名空间
        py::exec(code, global_vars, local_vars);
        return 0;
    }
    catch (const py::error_already_set& e) {
        //std::string errorType = py::str(e.type());
        //std::cout << "Python执行错误: " << e.what() << std::endl;
        //if (errorType.find("SyntaxError") != std::string::npos) {
        //    std::cout << "检测到语法错误！" << std::endl;
        //    return -2; // 语法错误
        //}
        return -1; // 其他错误
    }
}

int PyBind11PythonEngine::ConvertToMap(const py::dict& dict, std::unordered_map<std::string, int>& map_data)
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

wchar_t* PyBind11PythonEngine::StringToWchar(const std::string& str, UINT codePage)
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