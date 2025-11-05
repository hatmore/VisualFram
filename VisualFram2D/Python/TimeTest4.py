# print("开始进行处理Python程序")
import numpy
import sys
import importlib
import matplotlib
matplotlib.rcParams['font.sans-serif'] = ['SimHei']  # 中文
matplotlib.rcParams['axes.unicode_minus'] = False


# 全局变量用于统计 OK 良率
orderPredefined = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
ok_count = 132  # OK 次数
total_count = 136  # 总运行次数
yield_rate = 0  # 良率
resultJudge = 0 # OK/NG判断结果


def hello(name):
    print(f"你好, {name}!")


# 用于触发类型检测，可以是name和id，两种类型
def GetSearchType():
    return "name"


class SOPReportCreate():
    '''def __init__(self):
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
        return self.logInfo'''

    def Run(self):
        import os
        import pandas as pd
        import datetime
        global ok_count, total_count, yield_rate, resultJudge
        labelOrder= []
        labelDuration = []
        orderTime = []
        durationTime = []
        test_map_order = {
            1: 1757378314009,
            2: 1757380166229,
            3: 1757380190133,
            4: 1757380236709,
            5: 1757380236892,
            6: 1757380256086,
            7: 1757380236709,
            8: 1757380236892,
            9: 1757380256086,
            #10: 1757380236892,
            #11: 1757380256086
        }
        test_map_duration = {
            1: 1103,
            2: 5533,
            3: 66732,
            6: 33578,
            4: 35788,
            5: 4433,
            7: 1103,
            8: 5533,
            9: 66732,
            10: 33578,
            11: 57088
        }

        for key, value in test_map_order.items():
            labelOrder.append(key)
            if value > 99999999:
                # 输入数据为动作发生时间戳
                seconds = value / 1000
                time_obj = datetime.datetime.utcfromtimestamp(seconds)
                time_obj_beijing = time_obj + datetime.timedelta(hours=8)
                time_str = time_obj_beijing.strftime('%H:%M:%S')
                orderTime.append(time_str)

        for key, value in test_map_duration.items():
            labelDuration.append(key)
            durationTime.append(value / 1000)  # 转换为秒

        # SOP报表显示
        self.sopReport = self.GenerateReport(labelOrder, labelDuration, orderTime, durationTime)

        # 1、计算动作顺序的OK/NG判断
        resultJudge = all(id_val == orderPredefined[i] for i, id_val in enumerate(labelOrder)) if labelOrder else False
        if len(labelOrder) != len(orderPredefined):
            resultJudge = False
        # 2、读取本地保存的CSV数据
        csv_file = 'G:/PengSiyao/TestData/sop_report_data.csv'
        try:
            if os.path.exists(csv_file):
                df_existing = pd.read_csv(csv_file, encoding='utf-8-sig')
                # 提取 '总数量' 列中的最大值（忽略空值）
                total_count_values = df_existing['总数量'].dropna()
                if not total_count_values.empty:
                    total_count = int(total_count_values.max())
                # 提取 'OK数量' 列中的最大值（忽略空值）
                ok_count_values = df_existing['OK数量'].dropna()
                if not ok_count_values.empty:
                    ok_count = int(ok_count_values.max())
        except Exception as e:
            print(f"读取 CSV 文件时出错: {e}")
        # 3、更新 total_count 和 ok_count
        total_count += 1
        if resultJudge:
            ok_count += 1
        # 4、计算良率
        yield_rate = (ok_count / total_count * 100) if total_count > 0 else 0

        # 5、保存数据至CSV
        self.SaveToCSV(labelOrder, orderTime, labelDuration, durationTime)

    def SaveToCSV(self, labelOrder, orderTime, labelDuration, durationTime):
        import os
        import pandas as pd
        global ok_count, total_count, yield_rate

        csv_file = 'G:/PengSiyao/TestData/sop_report_data.csv'
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
                ax_ring.text(x, y, f'动作 {action_id}', fontproperties=font,
                             fontsize=title_font_size * 0.9, color='white', ha='center', va='center')
            # 添加右下角图例
            legend_y_start = 0.6  # 图例起始 y 坐标（右上角）
            line_height = 0.07  # 每行高度（基于 axs[0].transAxes）
            for i, (action_id, time, color) in enumerate(zip(labelOrder, orderTime, colors)):
                y_pos = legend_y_start - i * line_height
                rect = Rectangle((0.35, y_pos), 0.02, 0.03, transform=axs[0].transAxes,
                                 facecolor=color, edgecolor='white', linewidth=0.5, zorder=3)
                axs[0].add_patch(rect)
                axs[0].text(0.39, y_pos + 0.015, f'动作{action_id}:{time}', transform=axs[0].transAxes,
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
            status_text = 'OK' if True else 'NG'
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

        plt.show()
        return img_bgr  # 返回合并后的图像

if __name__ == "__main__":
    print(f"__main__")
    report = SOPReportCreate()
    report.Run()

