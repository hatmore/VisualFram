**中文** | [English](README.en.md)

# VisualFram（Visual Framework 2.0）

一个 **可视化任务流编程平台**，面向 **实时视觉处理与深度学习推理**。用户通过拖拽节点、连线即可搭建图像处理流水线（取流 → 推理 → ROI / 判定 → 通信 / 报表），无需编写代码。主程序负责节点编辑、任务流调度与插件管理，所有算法与 IO 能力都以 **插件 DLL** 的形式提供。

- 平台：Windows x64，Visual Studio 2019（MSVC v142），C++17，Qt 5.14.2
- 推理：TensorRT 8.6（YOLOv11 检测 / 姿态 / 分割、PoseC3D 动作识别）
- 并发：Intel TBB + Taskflow，多线程流水线执行
- 扩展：Python 3.10 脚本（pybind11）、Snap7 PLC、海康 HCNet 取流

## 目录结构

```
VisualFram/
├── README.md
├── docs/
│   ├── 项目分析报告.md            # 架构、插件系统、节点数据流、编译配置的完整说明
│   ├── 新增节点开发指南.md        # 如何开发一个新的节点插件
│   └── 动作持续时间计算.md        # FramImageDuration 插件的计算逻辑
└── VisualFram2D/
    ├── VisualFram2D.sln           # 主解决方案（主程序 + 全部插件）
    ├── Model/                     # TensorRT 引擎文件（*.plan）与示例模型
    ├── Project/                   # 示例任务流工程（*.pro，JSON 序列化）
    ├── Python/                    # 设备通信、SOP 报表等 Python 脚本
    ├── CaptureVideo/              # 测试视频
    └── VisualFram2D/
        ├── MianWindows/MianWindows/   # 主程序（exe）
        │   ├── NodeEditor/            #   节点编辑器（基于 QtNodes）
        │   ├── PlugiManager/          #   插件扫描与加载
        │   ├── AutoRunOperation/      #   任务流自动运行
        │   ├── TreeTaskFlowAndProject/#   工程 / 任务流树
        │   ├── CameraDevice/          #   相机设备
        │   ├── CommunicateDevice/     #   通信设备
        │   ├── PythonScript/          #   Python 脚本节点
        │   ├── ThreadPools/           #   线程池
        │   ├── Toolnterface/          #   插件接口定义（InterfacePlugin / ToolInterface）
        │   └── ...
        ├── Plugin/                    # 插件（每个一个 DLL，输出到 plugins/）
        │   ├── ImageProcess/
        │   │   ├── ImageSource/           # 图像源（相机 / 视频 / 图片）
        │   │   ├── YoloV11Detect/         # YOLOv11 目标检测
        │   │   ├── YoloV11Pose/           # YOLOv11 姿态估计
        │   │   ├── YoloV11Segment/        # YOLOv11 实例分割
        │   │   ├── PoseC3D/               # 基于骨骼的动作识别
        │   │   └── ImageMaskFilter/       # 掩膜过滤
        │   ├── Communication/
        │   │   ├── ExternalDeviceRead/    # 外部设备读取（PLC 等）
        │   │   └── ExternalDeviceWrite/   # 外部设备写入
        │   ├── ImageShow/                 # 图像显示（含 QtPropertyBrowser）
        │   ├── ImageROISelection/         # ROI 选择
        │   ├── ResultStateJudgment/       # 结果 / 状态判定
        │   ├── FramImageActionOrder/      # 动作顺序判定
        │   ├── FramImageDuration/         # 动作持续时间统计
        │   ├── ProcessControl/            # 流程控制
        │   └── StandardOperatingProcedureReport/  # SOP 报表
        └── ThirdParty/                # 第三方库（源码 / 头文件 / 运行库）
            ├── opencv4.5.4  TensorRT-8.6.0.12  oneapi-tbb-2021.12.0
            ├── Python310  Pybind11  Taskflow  Snap7  HCNet
            └── QtNodes  QtAdvancedDockingSystem  QGoodWindow
```

## 架构概览

```
┌──────────────────────────── MianWindows (exe) ────────────────────────────┐
│  NodeEditor(QtNodes)  TreeTaskFlow  AutoRunOperation  PlugiManager  ...   │
│                 ToolInterface / InterfacePlugin（插件 ABI）               │
└───────────▲──────────────▲──────────────▲──────────────▲──────────────────┘
            │              │              │              │   plugins/*.dll
      ImageSource     YoloV11*/PoseC3D   ROI/判定/时序   Communication/SOP
```

- **插件系统**：主程序启动时扫描 `plugins/` 目录，通过 `InterfacePlugin` 获取插件元信息并注册节点工厂；每个节点模型继承 `BaseNodeModel`，以数据包装类在节点间传递图像与结果。
- **数据流**：节点之间按连线传递 `std::shared_ptr` 包装的数据，任务流由 Taskflow 调度，在线程池中并行执行。
- **序列化**：任务流以 JSON 保存为 `.pro` 工程文件，节点参数通过属性浏览器编辑。

详细说明见 [docs/项目分析报告.md](docs/项目分析报告.md)。

## 构建

### 依赖

| 组件 | 版本 | 备注 |
|------|------|------|
| Visual Studio | 2019（v142） | 勾选「使用 C++ 的桌面开发」 |
| Qt | 5.14.2 msvc2017_64 | 需安装 Qt VS Tools 并在 VS 中配置 Qt 版本 |
| CUDA | 11.x | 与 TensorRT 8.6 匹配 |
| TensorRT | 8.6.0.12 | 仓库内 `ThirdParty/TensorRT-8.6.0.12` |
| OpenCV | 4.5.4 | 仓库内 `ThirdParty/opencv4.5.4` |
| Python | 3.10 | 仓库内 `ThirdParty/Python310` |

### 步骤

1. 克隆仓库（第三方库较大，建议 `git clone --depth 1`）。
2. 用 VS2019 打开 `VisualFram2D/VisualFram2D.sln`，在 Qt VS Tools 中指定 Qt 5.14.2 msvc2017_64。
3. 选择 `Debug|x64` 或 `Release|x64`，生成解决方案。所有项目通过 `PropertySheet/{Debug,Release}/*.props` 属性表引用第三方库。
4. 输出位于 `Build/Debug/VisualFram2.0.exe` 或 `Build/Release/VisualFram.exe`，插件 DLL 输出到同级 `plugins/`。
5. 运行前把 `Model/*.plan` 放到程序可访问的路径（`.plan` 与 GPU 架构绑定，`Model/JinKang3060` 为 RTX 3060 版本）。

> **注意**：历史上 `VisualFram2D/VisualFram2D/.gitignore` 忽略了 `*.vcxproj / *.sln`，因此各插件与主程序的 `.vcxproj`、`PropertySheet/*.props` 尚未入库。现已放开忽略规则，请在原开发机上执行 `git add` 把这些工程文件提交，否则新克隆无法直接编译。

## 开发新节点

参考 [docs/新增节点开发指南.md](docs/新增节点开发指南.md)：新建插件工程 → 实现 `InterfacePlugin` 与节点模型 → 定义输入/输出数据类型 → 编译到 `plugins/`。

## 许可

本项目暂未声明开源许可证；`ThirdParty/` 下各库遵循其自带许可。
