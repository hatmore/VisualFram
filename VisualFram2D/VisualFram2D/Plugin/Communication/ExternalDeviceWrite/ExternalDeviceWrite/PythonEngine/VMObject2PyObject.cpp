#include "VMObject2PyObject.h"


VMObjectStruct2PyObjectClass::VMObjectStruct2PyObjectClass()
{
}

VMObjectStruct2PyObjectClass::~VMObjectStruct2PyObjectClass()
{
}

void VMObjectStruct2PyObjectClass::GetVMObject(const PtrVMVector<PtrVMObject>& ptr)
{
    ptrVMVector = ptr;
}

void VMObjectStruct2PyObjectClass::VMVector2PyDict(const py::module& pybind11_calss, py::dict &py_dict)
{
    for (size_t i = 0; i < ptrVMVector->vmVector.size(); i++){
        PtrVMObject ptr_object =  ptrVMVector->vmVector[i];
        unsigned int node_id =  ptr_object->nodeID;
        auto str_node_id = std::to_string(node_id).c_str();
        VMDATATYPE cg_type = ptr_object->cgType;

        switch (cg_type)  {
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
            py_dict[str_node_id] = node_state;       
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
            py_dict[str_node_id] = node_state;    
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
            py_dict[str_node_id] = node_state;
        }
            break;
        case VM_POLYGON:
            break;
        case VM_MAT:
            break;
        case VM_VECTOR:
            break;
        case VM_UNORDERED_MAP:
            break;
        default:
            break;
        }

    }
}

void VMObjectStruct2PyObjectClass::VMObject2PyObject(const PtrVMObject& ptr_object)
{
    VMDATATYPE cg_type = ptr_object->cgType;
    switch (cg_type)
    {
    case VM_NULL:
        break;
    case VM_BOOL:
        break;
    case VM_POINT:
        break;
    case VM_RECTANGLE:
        break;
    case VM_POLYGON:
        break;
    case VM_MAT:
        break;
    case VM_VECTOR:
        break;
    case VM_UNORDERED_MAP:
        break;
    default:
        break;
    }

}