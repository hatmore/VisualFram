#print("开始进行处理Python程序")
import numpy
import sys
import importlib
import matplotlib
matplotlib.rcParams['font.sans-serif'] = ['SimHei']  # 中文
matplotlib.rcParams['axes.unicode_minus'] = False

# 全局变量用于统计 OK 良率
ok_count = 0  # OK 次数
total_count = 0  # 总运行次数

def hello(name):
    print(f"你好, {name}!")

###用于触发类型检测，可以是name和id，两种类型
def GetSearchType():
    return "name"

class SOPReportCreate():
    def __init__(self):
        self.pyVMObject = VMOjectClass
        self.sopReport = numpy.zeros((640, 640, 3), dtype=numpy.uint8)
        self.pyVMObjectInfo = ''
        self.logInfo = '调试信息：'

    def GetVMObjectInfo(self):
        self.pyVMObjectInfo = self.__AnalysisVMObjectInfo()
        return self.pyVMObjectInfo

    def GetSOPReport(self):
        return self.sopReport

    def GetLogInfo(self):
        return self.logInfo

    def Run(self):
        import datetime
        labelOrder= []
        labelDuration = []
        orderTime = []
        durationTime = []

        item_length = len(self.pyVMObject.items())
        if(item_length == 0):
            print("item_length: ", item_length)
            return
        print("item_length: ", item_length)
        for key, value in self.pyVMObject.items():
            #key, value = next(iter(self.pyVMObject.items()))
            self.__WriteLogInfo(f'key: {key}, vale: {str(value)}')
            pyList = value.pyList
            print("pyList: ", len(pyList))
            for py in pyList:
                self.__WriteLogInfo(f'id: {py.calssId}, vale: {(py.timeStamp)}')
                if (py.timeStamp > 99999999):
                    # 输入数据为动作发生时间戳
                    if(py.calssId < 0):
                        break
                    labelOrder.append(py.calssId)
                    time1 = py.timeStamp
                    seconds = time1 / 1000
                    time_obj = datetime.datetime.utcfromtimestamp(seconds)
                    time_obj_beijing = time_obj + datetime.timedelta(hours=8)
                    time_str = time_obj_beijing.strftime('%H:%M:%S')
                    orderTime.append(time_str)
                else:
                    # 输入数据为动作持续时间（ms）,转为s
                    if (py.calssId < 0):
                        break
                    labelDuration.append(py.calssId)
                    time2 = py.timeStamp / 1000
                    durationTime.append(time2)

        self.sopReport = self.GenerateReport(labelOrder, labelDuration , orderTime, durationTime)

    def __AnalysisVMObjectInfo(self):
        vmobject_info = []
        #print("VMOjectClass len: ", len(self.pyVMObject))
        for keys, values in self.pyVMObject.items():
            self.__WriteLogInfo(f'key: {keys}, vale: {type(values)}')
            info = '键: ' + keys + ' 键:' + str(values) + ";\n"
            vmobject_info.append(info)
        string = ''.join(vmobject_info)
        return string

    def __WriteLogInfo(self, info: str):
        self.logInfo = self.logInfo + '\n' + info

    def GenerateReport(self, labelOrder, labelDuration, orderTime, durationTime):
        global ok_count, total_count  # 声明使用全局变量
        import cv2
        import numpy as np
        from matplotlib import cm
        from matplotlib.ticker import FuncFormatter
        from matplotlib.font_manager import FontProperties
        import matplotlib.pyplot as plt
        from matplotlib.patches import Rectangle

        # 更新总运行次数
        total_count += 1
        font = FontProperties(fname="C:/Windows/Fonts/simhei.ttf", size=16)
        plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

        # 创建画布：2×1 垂直布局
        fig, axs = plt.subplots(2, 1, figsize=(16, 8), dpi=120, gridspec_kw={'height_ratios': [1, 1]})
        plt.subplots_adjust(top=0.95, bottom=0.05, hspace=0.2)

        # 上半部分：动作顺序统计表格 + OK/NG
        axs[0].set_title('动作顺序统计结果', fontproperties=font, fontsize=20, color='orange', y=1.0, x=0.25)
        # 在 inner_rect 上方添加新标题或文字
        axs[0].text(0.75, 1.0, '结果统计',
                    transform=axs[0].transAxes,
                    fontproperties=font,
                    fontsize=20,
                    color='orange',
                    ha='left',
                    va='bottom')
        plt.subplots_adjust(top=0.95)

        if labelOrder and orderTime:
            # 创建表格数据
            table_data = [[i + 1, f'检测到动作{id}，发生的时间为{time}'] for i, (id, time) in enumerate(zip(labelOrder, orderTime))]
            # 添加表格，调整 bbox 使表格偏左
            table = axs[0].table(cellText=table_data,
                                 colLabels=['顺序', '动作发生的时间'],
                                 loc='center left',
                                 cellLoc='center',
                                 colWidths=[0.1, 0.5],  # 调整列宽，留出右边空间
                                 bbox=[0.0, 0.0, 0.55, 0.95])  # 表格占左边 55% 宽度
            # 设置表格样式
            # 设置表格样式
            table.auto_set_font_size(False)
            table.set_fontsize(12)
            for (row, col), cell in table.get_celld().items():
                cell.set_text_props(fontproperties=font)
                if col == 1 and row > 0:
                    i = row - 1
                    id_val = labelOrder[i]
                    cell.set_text_props(color='green' if id_val == i + 1 else 'red')

            # OK/NG 显示，放在右上角
            all_ids_match = all(id_val == i + 1 for i, id_val in enumerate(labelOrder))
            if all_ids_match:
                status_text = 'OK'
                ok_count += 1
            else:
                status_text = 'NG'

            # 注释：根据 status_text 设置 inner_rect 背景颜色，OK 为绿色，NG 为红色
            inner_rect_color = 'green' if status_text == 'OK' else 'red'
            # 先绘制内层矩形框（绿色或红色背景）
            # 注释：固定尺寸，宽 0.15 高 0.15，居中于 (0.85, 0.85)，zorder=1 确保在底层
            rect_width = 0.1
            rect_height = 0.25
            rect_x = 0.899
            rect_y = 0.7
            inner_rect = Rectangle((rect_x, rect_y), rect_width, rect_height,
                                   transform=axs[0].transAxes,
                                   edgecolor='black', facecolor=inner_rect_color, linewidth=2, zorder=1)
            axs[0].add_patch(inner_rect)
            # 绘制 OK/NG 文本，字体颜色为白色
            # 注释：color='white' 设置文本为白色，zorder=2 确保在矩形框之上
            axs[0].text(0.95, 0.82, status_text,
                        transform=axs[0].transAxes,
                        fontproperties=font,
                        fontsize=40,
                        color='white',
                        weight='bold',
                        ha='center',
                        va='center',
                        zorder=2)
            # 绘制总计数文本
            axs[0].text(0.76, 0.55, f'总数: {total_count}',
                        transform=axs[0].transAxes,
                        fontproperties=font,
                        fontsize=30,
                        color='black',
                        weight='bold',
                        ha='center',
                        va='center',
                        zorder=2)
            # 计算良率
            yield_rate = (ok_count / total_count * 100) if total_count > 0 else 0
            # 添加外层矩形框围绕整个右上角区域
            # 注释：覆盖右上角（x=0.7 到 1.0，y=0.7 到 1.0），facecolor='none' 仅显示边框
            outer_rect = Rectangle((0.6, 0.002), 0.398, 0.95,
                                   transform=axs[0].transAxes,
                                   edgecolor='black', facecolor='none', linewidth=2, zorder=0)
            axs[0].add_patch(outer_rect)
            # 添加良率文本
            axs[0].text(0.8, 0.4, f'良率: {yield_rate:.2f}%',
                        transform=axs[0].transAxes,
                        fontproperties=font,
                        fontsize=30,
                        color='green',
                        ha='center',
                        va='center',
                        zorder=2)
        else:
            axs[0].text(0.5, 0.5, '无数据', transform=axs[0].transAxes,
                        fontproperties=font, fontsize=30, color='r', ha='center', va='center')
        axs[0].axis('off')

        # 下半部分：动作持续时间柱状图
        if labelDuration and durationTime:
            colors2 = cm.plasma(np.linspace(0, 1.0, len(durationTime)))
            bars2 = axs[1].bar(range(len(labelDuration)), durationTime, color=colors2, edgecolor=colors2, linewidth=1.2)
            for bar, val in zip(bars2, durationTime):
                height = bar.get_height()
                if height > 0:
                    axs[1].text(bar.get_x() + bar.get_width() / 2, height + 0.5, f'{val:.2f}',
                                ha='center', va='bottom', fontproperties=font, fontsize=18, color='r')
            axs[1].set_xticks(range(len(labelDuration)))
            axs[1].set_xticklabels(labelDuration, fontproperties=font, fontsize=12)
            axs[1].set_ylabel('持续时间', fontproperties=font, fontsize=14, color='k')
            axs[1].set_xlabel('动作名称/ID', fontproperties=font, fontsize=14, color='k')
            axs[1].set_title('动作持续时间统计结果', fontproperties=font, fontsize=20, color='orange')
            axs[1].yaxis.set_major_formatter(FuncFormatter(lambda y, _: f'{y:.2f} 秒'))
            axs[1].spines['top'].set_visible(False)
            axs[1].spines['right'].set_visible(False)
            axs[1].grid(axis='y', linestyle='--', alpha=0.7)
            axs[1].set_ylim(0, max(durationTime) * 1.5)
        else:
            axs[1].text(0.5, 0.5, '无数据', transform=axs[1].transAxes,
                        fontproperties=font, fontsize=30, color='r', ha='center', va='center')
            axs[1].set_title('动作持续时间统计结果', fontproperties=font, fontsize=20, color='orange')
            axs[1].axis('off')

        # 5 转换为图像
        fig.canvas.draw()
        width, height = fig.canvas.get_width_height()
        img_rgba = np.frombuffer(fig.canvas.tostring_argb(), dtype=np.uint8).reshape(height, width, 4)
        img_rgba = img_rgba[:, :, [1, 2, 3, 0]]
        img_bgr = cv2.cvtColor(img_rgba, cv2.COLOR_RGBA2BGR)

        return img_bgr  # 返回合并后的图像

        # 6 显示图表
        #plt.tight_layout()
        #plt.show()
        #plt.close(fig)  # 关闭画布以释放资源


if __name__ == "__main__":
    print(f"__main__")
    #report = SOPReportCreate()
    #report.Run()

