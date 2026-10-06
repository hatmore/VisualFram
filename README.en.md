[中文](README.md) | **English**

# VisualFram (Visual Framework 2.0)

A **visual task-flow programming platform** for **real-time vision processing and deep-learning inference**. Users build image-processing pipelines (capture → inference → ROI / judgement → communication / reports) by dragging nodes and wiring them together, without writing code. The main application handles node editing, task-flow scheduling and plugin management; every algorithm and I/O capability is delivered as a **plugin DLL**.

- Platform: Windows x64, Visual Studio 2019 (MSVC v142), C++17, Qt 5.14.2
- Inference: TensorRT 8.6 (YOLOv11 detection / pose / segmentation, PoseC3D action recognition)
- Concurrency: Intel TBB + Taskflow, multi-threaded pipeline execution
- Extensibility: Python 3.10 scripting (pybind11), Snap7 PLC, Hikvision HCNet streams

## Layout

```
VisualFram/
├── README.md
├── docs/
│   ├── 项目分析报告.md            # full write-up: architecture, plugin system, node data flow, build config
│   ├── 新增节点开发指南.md        # how to develop a new node plugin
│   └── 动作持续时间计算.md        # how the FramImageDuration plugin computes durations
└── VisualFram2D/
    ├── VisualFram2D.sln           # main solution (application + all plugins)
    ├── Model/                     # TensorRT engines (*.plan) and sample models
    ├── Project/                   # sample task-flow projects (*.pro, JSON)
    ├── Python/                    # device communication and SOP report scripts
    ├── CaptureVideo/              # test video
    └── VisualFram2D/
        ├── MianWindows/MianWindows/   # main application (exe)
        │   ├── NodeEditor/            #   node editor (built on QtNodes)
        │   ├── PlugiManager/          #   plugin discovery and loading
        │   ├── AutoRunOperation/      #   automatic task-flow execution
        │   ├── TreeTaskFlowAndProject/#   project / task-flow tree
        │   ├── CameraDevice/          #   camera devices
        │   ├── CommunicateDevice/     #   communication devices
        │   ├── PythonScript/          #   Python script node
        │   ├── ThreadPools/           #   thread pool
        │   ├── Toolnterface/          #   plugin interfaces (InterfacePlugin / ToolInterface)
        │   └── ...
        ├── Plugin/                    # plugins (one DLL each, built into plugins/)
        │   ├── ImageProcess/
        │   │   ├── ImageSource/           # image source (camera / video / image)
        │   │   ├── YoloV11Detect/         # YOLOv11 object detection
        │   │   ├── YoloV11Pose/           # YOLOv11 pose estimation
        │   │   ├── YoloV11Segment/        # YOLOv11 instance segmentation
        │   │   ├── PoseC3D/               # skeleton-based action recognition
        │   │   └── ImageMaskFilter/       # mask filtering
        │   ├── Communication/
        │   │   ├── ExternalDeviceRead/    # read from external devices (PLC etc.)
        │   │   └── ExternalDeviceWrite/   # write to external devices
        │   ├── ImageShow/                 # image display (with QtPropertyBrowser)
        │   ├── ImageROISelection/         # ROI selection
        │   ├── ResultStateJudgment/       # result / state judgement
        │   ├── FramImageActionOrder/      # action order checking
        │   ├── FramImageDuration/         # action duration statistics
        │   ├── ProcessControl/            # flow control
        │   └── StandardOperatingProcedureReport/  # SOP reports
        └── ThirdParty/                # third-party libraries (sources / headers / runtimes)
            ├── opencv4.5.4  TensorRT-8.6.0.12  oneapi-tbb-2021.12.0
            ├── Python310  Pybind11  Taskflow  Snap7  HCNet
            └── QtNodes  QtAdvancedDockingSystem  QGoodWindow
```

## Architecture

```
┌──────────────────────────── MianWindows (exe) ────────────────────────────┐
│  NodeEditor(QtNodes)  TreeTaskFlow  AutoRunOperation  PlugiManager  ...   │
│                 ToolInterface / InterfacePlugin (plugin ABI)              │
└───────────▲──────────────▲──────────────▲──────────────▲──────────────────┘
            │              │              │              │   plugins/*.dll
      ImageSource     YoloV11*/PoseC3D   ROI/judge/time  Communication/SOP
```

- **Plugin system**: at start-up the application scans `plugins/`, reads plugin metadata through `InterfacePlugin` and registers node factories. Every node model derives from `BaseNodeModel` and passes images and results between nodes through data wrapper classes.
- **Data flow**: data travels along node connections as `std::shared_ptr` wrappers; task flows are scheduled by Taskflow and executed in parallel on the thread pool.
- **Serialisation**: task flows are saved as JSON `.pro` project files; node parameters are edited through the property browser.

See [docs/项目分析报告.md](docs/项目分析报告.md) for details (Chinese).

## Build

### Dependencies

| Component | Version | Notes |
|-----------|---------|-------|
| Visual Studio | 2019 (v142) | "Desktop development with C++" workload |
| Qt | 5.14.2 msvc2017_64 | Qt VS Tools installed and the Qt version configured in VS |
| CUDA | 11.x | matching TensorRT 8.6 |
| TensorRT | 8.6.0.12 | in-tree under `ThirdParty/TensorRT-8.6.0.12` |
| OpenCV | 4.5.4 | in-tree under `ThirdParty/opencv4.5.4` |
| Python | 3.10 | in-tree under `ThirdParty/Python310` |

### Steps

1. Clone the repository (third-party libraries are large; `git clone --depth 1` is recommended).
2. Open `VisualFram2D/VisualFram2D.sln` in VS2019 and point Qt VS Tools at Qt 5.14.2 msvc2017_64.
3. Select `Debug|x64` or `Release|x64` and build the solution. All projects reference third-party libraries through property sheets in `PropertySheet/{Debug,Release}/*.props`.
4. Output is `Build/Debug/VisualFram2.0.exe` or `Build/Release/VisualFram.exe`, with plugin DLLs in the sibling `plugins/` directory.
5. Before running, place `Model/*.plan` where the application can reach them (`.plan` files are tied to the GPU architecture; `Model/JinKang3060` targets an RTX 3060).

> **Note**: `VisualFram2D/VisualFram2D/.gitignore` used to ignore `*.vcxproj` and `*.sln`, so the per-project `.vcxproj` files and `PropertySheet/*.props` have never been committed. The ignore rules have been relaxed; please `git add` those project files from the original development machine, otherwise a fresh clone cannot be built as-is.

## Writing a new node

See [docs/新增节点开发指南.md](docs/新增节点开发指南.md) (Chinese): create a plugin project → implement `InterfacePlugin` and the node model → define input/output data types → build into `plugins/`.

## License

No license has been declared for this project yet. Libraries under `ThirdParty/` keep their own licenses.
