#print("=== Python脚本开始执行 ===")
import sys


def hello(name):
    print(f"你好, {name}!")

###用于触发类型检测，可以是name和id，两种类型
def GetSearchType():
    return "name"


# 发送节点名称进行节点操作：发送字符，表示操作方式
# -1：表示暂停，流程
# 0：表示自动循环
# 1：表示循环一次
# 2：代表循环两次，一次类推
class CommunicateDevice:
    def __init__(self):
        self.reviceData = ""


    def SetReviceData(self, revice_data:str):
        self.reviceData = revice_data

    def GetReviceData(self) -> str:
        return self.reviceData

    def GetResult(self):
        parse_dict = {}
        list_string = self.__SplitStrings(self.reviceData, ';')
        for string in list_string:
             list_str = self.__SplitStrings(string, ':')
             if(len(list_str) == 2):
                parse_dict[list_str[0]] = list_str[1]

        result_dict = {}
        for key,value in parse_dict.items():
            if(key == "110"):
                if(value == "0"):
                    result_dict['flow0'] = 0
                elif(value == "-1"):
                    result_dict['flow0'] = -1
                else:
                    result_dict['flow0'] = int(value)
            elif (key == "210"):
                if (value == "0"):
                    #写通信节点停止发送数据
                    result_dict['sendDataControl'] = 0
                elif (value == "1"):
                    #写通信节点开始发送数据
                    result_dict['sendDataControl'] = 1
                else:
                    result_dict['sendDataControl'] = int(value)
            elif (key == "211"):
                if (value == "0"):
                    # 数据发送完成信号置false
                    result_dict['sendDataComplete'] = 0
                elif (value == "1"):
                    # 数据发送完成信号置true
                    result_dict['sendDataComplete'] = 1
                else:
                    result_dict['sendDataComplete'] = int(value)
            elif (key == "112"):
                if (value == "0"):
                    result_dict['flow1'] = 0
                elif (value == "-1"):
                    result_dict['flow1'] = -1
                else:
                    result_dict['flow0'] = int(value)
        return result_dict


    def __SplitStrings(self, data, symbol):
        list_string = data.split(symbol)
        return list_string


if __name__ == "__main__":
    string = "110:1"
    comm_device = CommunicateDevice()
    comm_device.SetReviceData(string)
    map = comm_device.GetResult()