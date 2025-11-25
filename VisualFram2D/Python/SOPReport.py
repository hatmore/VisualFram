#print("开始进行处理Python程序")
import numpy
import matplotlib
matplotlib.rcParams['font.sans-serif'] = ['SimHei']  # 中文
matplotlib.rcParams['axes.unicode_minus'] = False

# 全局变量用于统计 OK 良率
orderPredefined = [1, 2, 3, 4, 5]
ok_count = 0  # OK 次数
total_count = 0  # 总运行次数
yield_rate = 0  # 良率
resultJudge = 0 # OK/NG判断结果
last_date = None  # 记录上一次运行的日期

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
        import os
        import pandas as pd
        import datetime
        global ok_count, total_count, yield_rate, orderPredefined, resultJudge, last_date

        today = datetime.datetime.now().strftime('%Y-%m-%d')
        # 检查是否为新的一天，如果是则清空计数
        if last_date != today:
            ok_count = 0
            total_count = 0
            last_date = today

        labelOrder= []
        labelDuration = []
        orderTime = []
        durationTime = []

        item_length = len(self.pyVMObject.items())
        if (item_length == 0):
            print("item_length: ", item_length)
            return
        print("item_length: ", item_length)
        for key, value in self.pyVMObject.items():
            # key, value = next(iter(self.pyVMObject.items()))
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

        # 1、读取本地 CSV 文件中保存的数据，并判断数据是否更新
        csv_file = f'D:/app/TestData/sop_report_data_{today}.csv'
        try:
            if os.path.exists(csv_file):
                df_existing = pd.read_csv(csv_file, encoding='utf-8-sig')
                # 提取“动作发生的时间”列，获取最后一行
                order_time_values = df_existing['动作发生的时间'].dropna()
                if not order_time_values.empty:
                    last_order_time = order_time_values.iloc[-1]
                    # 比较当前 orderTime 与上一次的 orderTime
                    if orderTime[-1] == last_order_time:
                        return
                # 提取“总数量”和“OK数量”
                total_count_values = df_existing['总数量'].dropna()
                if not total_count_values.empty:
                    total_count = int(total_count_values.max())
                ok_count_values = df_existing['OK数量'].dropna()
                if not ok_count_values.empty:
                    ok_count = int(ok_count_values.max())
        except Exception as e:
            print(f"读取 CSV 文件时出错: {e}")

        # 2、计算动作顺序的OK/NG判断
        if len(labelOrder) < len(orderPredefined):
            resultJudge = False
        else:
            resultJudge = (labelOrder[:len(orderPredefined)] == orderPredefined)

        # 3、更新 total_count 和 ok_count
        total_count += 1
        if resultJudge:
            ok_count += 1
        # 4、计算良率
        yield_rate = (ok_count / total_count * 100) if total_count > 0 else 0

        # 5、渲染显示报表
        self.sopReport = self.GenerateReport(labelOrder, labelDuration, orderTime, durationTime)

        # 6、保存数据至CSV
        self.SaveToCSV(labelOrder, orderTime, labelDuration, durationTime)


    def SaveToCSV(self, labelOrder, orderTime, labelDuration, durationTime):
        import os
        import pandas as pd
        import datetime
        global ok_count, total_count, yield_rate

        today = datetime.datetime.now().strftime('%Y-%m-%d')
        csv_file = f'D:/app/TestData/sop_report_data_{today}.csv'
        # 保持统一长度
        max_length = max(len(labelOrder), len(labelDuration))
        data = {
            '检测动作': labelOrder + [''] * (max_length - len(labelOrder)),
            '动作发生的时间': orderTime + [''] * (max_length - len(orderTime)),
            '检测动作_持续': labelDuration + [''] * (max_length - len(labelDuration)),
            '动作持续的时间': durationTime + [''] * (max_length - len(durationTime)),
            'OK数量': [ok_count] + [''] * (max_length - 1),
            '总数量': [total_count] + [''] * (max_length - 1),
            '良率': [f'{yield_rate:.2f}%'] + [''] * (max_length - 1)
        }
        df = pd.DataFrame(data)

        # 添加新数据
        try:
            if os.path.exists(csv_file):
                df.to_csv(csv_file, mode='a', header=False, index=False, encoding='utf-8-sig')
            else:
                df.to_csv(csv_file, mode='w', header=True, index=False, encoding='utf-8-sig')
        except Exception as e:
            print(f"保存 CSV 文件时出错: {e}")


    def __AnalysisVMObjectInfo(self):
        vmobject_info = []
        # print("VMOjectClass len: ", len(self.pyVMObject))
        for keys, values in self.pyVMObject.items():
            self.__WriteLogInfo(f'key: {keys}, vale: {type(values)}')
            info = '键: ' + keys + ' 键:' + str(values) + ";\n"
            vmobject_info.append(info)
        string = ''.join(vmobject_info)
        return string

    def __WriteLogInfo(self, info: str):
        self.logInfo = self.logInfo + '\n' + info

    def GenerateReport(self, labelOrder, labelDuration, orderTime, durationTime):
        global ok_count, total_count, yield_rate, resultJudge  # 声明使用全局变量
        import numpy as np
        import cv2
        import matplotlib.pyplot as plt
        from matplotlib.font_manager import FontProperties
        from matplotlib.patches import Wedge, Rectangle
        from matplotlib import cm  # 新增：用于颜色映射

        # 设置字体
        font = FontProperties(fname="C:/Windows/Fonts/simhei.ttf")

        # 创建画布，动态设置大小
        fig_width, fig_height = 12, 7  # 基础尺寸
        fig, axs = plt.subplots(2, 1, figsize=(fig_width, fig_height), dpi=150,
                                gridspec_kw={'height_ratios': [4, 3]})

        # 计算动态字体大小（基于画布宽度）
        base_font_size = fig_width * 1.5  # 基础字体大小，按画布宽度比例调整
        title_font_size = base_font_size * 1.2  # 标题字体稍大
        label_font_size = base_font_size * 0.9  # 标签字体稍小

        # 设置全局背景色和字体
        fig.patch.set_facecolor('#233342')  # 整体背景色 RGB(35, 51, 66)
        plt.rcParams['axes.facecolor'] = '#3D6086'  # 替换seaborn的背景色设置
        plt.rcParams['font.family'] = 'SimHei'  # 设置全局字体
        plt.subplots_adjust(top=0.90, bottom=0.10, hspace=0.3)  # 增加 bottom 边距以显示 x 轴标签

        # 上方布局：左侧环形图，右侧 OK/NG
        axs[0].set_facecolor('#233342')  # 上方区域与画布背景色一致
        ax_ring = axs[0].inset_axes([-0.02, 0.15, 0.4, 0.7], facecolor='#3D6086')  # 环形图
        ring_border = Rectangle((0, 0), 0.55, 1, transform=axs[0].transAxes,
                                facecolor='#3D6086', edgecolor='white', linewidth=2, zorder=0)
        axs[0].add_patch(ring_border)

        # 左侧：环形图（按 labelOrder 顺序逆时针）
        if labelOrder and orderTime:
            ax_ring.set_title('动作顺序时间统计', fontproperties=font, fontsize=title_font_size,
                              color='white', pad=37, x=0.8)  # 调整 pad 防止标题溢出
            ax_ring.set_aspect('equal')  # 确保正圆
            num_actions = len(labelOrder)
            angles = [360 / num_actions] * num_actions
            # 使用matplotlib的hsv颜色映射替换seaborn的hls
            colors = [cm.hsv(i / num_actions) for i in range(num_actions)]

            # 按 labelOrder 顺序逆时针排列
            for i, (action_id, time, color) in enumerate(zip(labelOrder, orderTime, colors)):
                start_angle = sum(angles[:i])  # 从 0 度开始
                end_angle = start_angle + angles[i]
                wedge = Wedge((0.5, 0.5), 0.4, 360 - end_angle, 360 - start_angle, width=0.1,
                              facecolor=color, edgecolor='white')
                ax_ring.add_patch(wedge)
                label_angle = np.deg2rad(360 - (start_angle + angles[i] / 2))
                x = 0.5 + 0.45 * np.cos(label_angle)
                y = 0.5 + 0.45 * np.sin(label_angle)
                ax_ring.text(x, y, f'拧紧 {action_id}', fontproperties=font,
                             fontsize=title_font_size * 0.9, color='white', ha='center', va='center')
            # 添加右下角图例
            legend_y_start = 0.6  # 图例起始 y 坐标（右上角）
            line_height = 0.07  # 每行高度（基于 axs[0].transAxes）
            for i, (action_id, time, color) in enumerate(zip(labelOrder, orderTime, colors)):
                y_pos = legend_y_start - i * line_height
                rect = Rectangle((0.35, y_pos), 0.02, 0.03, transform=axs[0].transAxes,
                                 facecolor=color, edgecolor='white', linewidth=0.5, zorder=3)
                axs[0].add_patch(rect)
                axs[0].text(0.39, y_pos + 0.015, f'拧紧{action_id}:{time}', transform=axs[0].transAxes,
                            fontproperties=font, fontsize=label_font_size * 0.8, color='white',
                            ha='left', va='center', zorder=5)
            ax_ring.axis('off')
        else:
            ax_ring.text(0.5, 0.5, '无数据', fontproperties=font, fontsize=title_font_size,
                         color='white', ha='center', va='center')
            ax_ring.axis('off')

        # 右侧：OK/NG 显示
        okng_border = Rectangle((0.65, 0), 0.35, 1, transform=axs[0].transAxes,
                                facecolor='#3D6086', edgecolor='white', linewidth=2, zorder=0)
        axs[0].add_patch(okng_border)
        if labelOrder:
            status_text = 'OK' if resultJudge else 'NG'
            status_color = '#28A745' if status_text == 'OK' else '#DC3545'
            axs[0].text(0.77, 1.12, '结果统计', transform=axs[0].transAxes,
                        fontproperties=font, fontsize=title_font_size, color='white',
                        ha='left', va='top')
            inner_rect = Rectangle((0.9, 0.75), 0.1, 0.25, transform=axs[0].transAxes,
                                   edgecolor='white', facecolor=status_color, linewidth=2, zorder=2)
            axs[0].add_patch(inner_rect)
            shadow = Rectangle((0.91, 0.74), 0.1, 0.25, transform=axs[0].transAxes,
                               facecolor='black', alpha=0.2, zorder=1)
            axs[0].add_patch(shadow)
            axs[0].text(0.95, 0.87, status_text, transform=axs[0].transAxes,
                        fontproperties=font, fontsize=title_font_size * 2.5, color='white',
                        weight='bold', ha='center', va='center', zorder=3)
            axs[0].text(0.75, 0.55, f'总数: {total_count}', transform=axs[0].transAxes,
                        fontproperties=font, fontsize=title_font_size, color='white',
                        ha='center', va='center', zorder=3)
            axs[0].text(0.75, 0.45, f'良率: {yield_rate:.2f}%', transform=axs[0].transAxes,
                        fontproperties=font, fontsize=title_font_size, color='white',
                        ha='center', va='center', zorder=3)
        else:
            axs[0].text(0.75, 0.5, '无数据', transform=axs[0].transAxes,
                        fontproperties=font, fontsize=title_font_size, color='white',
                        ha='center', va='center')
        axs[0].axis('off')
        axs[1].grid(False)  # 删除表格线

        # 下方：柱状图（仅保留横线）
        axs[1].set_facecolor('#3D6086')  # 背景色 RGB(61, 96, 134)
        if labelDuration and durationTime:
            axs[1].set_title('动作持续时间统计', fontproperties=font, fontsize=title_font_size,
                             color='white', pad=10)
            # 使用matplotlib的Blues颜色映射替换seaborn的Blues
            colors = cm.Blues(np.linspace(0.3, 0.9, len(durationTime)))
            bars = axs[1].bar(range(len(labelDuration)), durationTime, color=colors,
                              edgecolor='white', linewidth=1.5)

            for bar in bars:
                x, y, width, height = bar.get_x(), bar.get_y(), bar.get_width(), bar.get_height()
                shadow = Rectangle((x + 0.02, y - 0.02), width, height, color='black', alpha=0.2)
                axs[1].add_patch(shadow)

            for bar, val in zip(bars, durationTime):
                axs[1].text(bar.get_x() + bar.get_width() / 2, val + 0.5, f'{val:.2f}',
                            ha='center', va='bottom', fontproperties=font,
                            fontsize=label_font_size, color='white')

            axs[1].set_xticks(range(len(labelDuration)))
            axs[1].set_xticklabels(labelDuration, fontproperties=font, fontsize=label_font_size,
                                   color='white')
            axs[1].set_ylabel('持续时间 (秒)', fontproperties=font, fontsize=label_font_size,
                              color='white')
            axs[1].tick_params(axis='y', colors='white')
            axs[1].set_xlabel('动作名称/ID', fontproperties=font, fontsize=label_font_size,
                              color='white', labelpad=5)
            axs[1].spines['top'].set_visible(False)
            axs[1].spines['right'].set_visible(False)
            axs[1].spines['left'].set_color('white')
            axs[1].spines['bottom'].set_color('white')
            axs[1].grid(axis='y', linestyle='--', alpha=0.5, color='white')
            axs[1].set_ylim(0, max(durationTime) * 1.3)
        else:
            # 无数据时显示柱状图框架，并在中间显示“无数据”
            axs[1].set_xticks([])
            axs[1].set_yticks([])
            axs[1].set_ylabel('持续时间 (秒)', fontproperties=font, fontsize=label_font_size,
                              color='white')
            axs[1].set_xlabel('动作名称/ID', fontproperties=font, fontsize=label_font_size,
                              color='white', labelpad=5)
            axs[1].spines['top'].set_visible(False)
            axs[1].spines['right'].set_visible(False)
            axs[1].spines['left'].set_color('white')
            axs[1].spines['bottom'].set_color('white')
            axs[1].text(0.5, 0.5, '无数据', fontproperties=font, fontsize=title_font_size,
                        color='white', ha='center', va='center', transform=axs[1].transAxes)

        # 转换为图像
        fig.canvas.draw()
        width, height = fig.canvas.get_width_height()
        img_rgba = np.frombuffer(fig.canvas.tostring_argb(), dtype=np.uint8).reshape(height, width, 4)
        img_rgba = img_rgba[:, :, [1, 2, 3, 0]]
        img_bgr = cv2.cvtColor(img_rgba, cv2.COLOR_RGBA2BGR)

        return img_bgr  # 返回合并后的图像


if __name__ == "__main__":
    print(f"__main__")

