print("=== Python脚本开始执行 ===")
import sys
import Pybind11Class




def hello(name):
    print(f"你好, {name}!")

###用于触发类型检测，可以是name和id，两种类型
def GetSearchType():
    return "name"

class CommunicateDeviceWrite():
    def __init__(self):
        self.pyVMObject = VMOjectClass
        self.writeData = ""
        self.pyVMObjectInfo = ''
        self.logInfo = '调试信息：'

    def GetVMObjectInfo(self):
        pyVMObjectInfo = self.__AnalysisVMObjectInfo()
        return pyVMObjectInfo

    def GetWriteData(self):
        return self.writeData

    def GetLogInfo(self):
        return self.logInfo

    def Run(self):
        import datetime
        print("VMOjectClass len: ", len(self.pyVMObject))
        vmobject_info = []
        for keys, values in self.pyVMObject.items():
            info = 'keyAAAAAAAAAAAAAAAAAAA: ' + keys + ' vaAAAAAAAAAAAAAAAAaaalue:' + str(values) + ";\n"
            self.__WriteLogInfo(f'A: {keys}, 值: {values}')
            vmobject_info.append(info)
        self.writeData = ''.join(vmobject_info)


    def __AnalysisVMObjectInfo(self):
        vmobject_info = []
        print("VMOjectClass len: ", len(self.pyVMObject))
        for keys, values in self.pyVMObject.items():
            self.__WriteLogInfo(f'键: {keys}, 值: {values}')
            info = '键: ' + keys + ' 键:' + str(values) + ";\n"
            vmobject_info.append(info)
        string = ''.join(vmobject_info)
        return string
        
    def __WriteLogInfo(self, info:str):
        self.logInfo = self.logInfo + '\n' + info


if __name__ == "__main__":
    print(f"预编译测试")
    # PyVMObject = Pybind11Class.PyVMObject
    # print(PyVMObject)
    # print("cpp_module2: ", len(VMOjectClass))
    # vmobject_info = []
    # for keys, values in VMOjectClass.items():
    #     print(f'键: {keys}, 值: {values}')
    #     print(values.nodeID)
    #     info = 'key: ' + keys + ' value:'+ str(values) + ";\n"
    #     vmobject_info.append(info)
    # string = ''.join(vmobject_info)
    # print(string)











