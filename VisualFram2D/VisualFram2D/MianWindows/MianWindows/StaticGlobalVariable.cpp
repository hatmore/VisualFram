#include "StaticGlobalVariable.h"


//FramTaskFlowViewWidget* StaticGlobalVariable::framTaskFlowViewWidgetStatic;    ///流图

PtrSystemSetParam StaticGlobalVariable::ptrSystemSetParam = std::make_shared<SystemSetParam>();

std::chrono::milliseconds StaticGlobalVariable::threadTimeOut = std::chrono::milliseconds(1000 * 100);

ToolNodeDataInteract::Ptr StaticGlobalVariable::ptrToolNodeDataInteract = std::make_shared<ToolNodeDataInteract>();