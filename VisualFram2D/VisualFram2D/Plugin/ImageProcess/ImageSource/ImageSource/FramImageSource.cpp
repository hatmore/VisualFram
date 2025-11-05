#include "FramImageSource.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <stdlib.h>
#include <string>
#include <io.h>

FramImageSource::FramImageSource(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);    
    Initial(); 
}

FramImageSource::~FramImageSource()
{
    //if (ptrMapNodesOutData  !=  nullptr) {
    //    auto iter = ptrMapNodesOutData->find(this->nodeId);
    //    if (iter != ptrMapNodesOutData->end()) {
    //        ptrMapNodesOutData->erase(iter);
    //    }
    //}
}

void FramImageSource::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitialExserializeUi();
}

void FramImageSource::InitialNodeData()
{
    ptrVMMat = std::make_shared<VMMat>();
    ptrOutNodeVMMat = std::make_shared<BaseNodeData>();
    ptrOutNodeVMMat->SetCGObjectNodeFlow(ptrVMMat);

    FlowNodeAndName flow_node_and_name;
    flow_node_and_name.nodeName = "图像";
    flow_node_and_name.ptrNodedata = ptrOutNodeVMMat;

    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, flow_node_and_name));
}

void FramImageSource::InitialImageShow()
{
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint    // | Qt::WindowMaximizeButtonHint
    //    | Qt::WindowCloseButtonHint);
    this->setWindowTitle("图像源");
    this->setWindowIcon(QIcon(":/FramImageSource/Image/DLCW2.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);


    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    connect(this, &FramImageSource::SendNodeRuningStateSyncSig, this, &FramImageSource::AcceptSelfRunringStateSlot);

}

void FramImageSource::InitialPushButton()
{
   // ui.radioButtonLocaImage->click();
    connect(ui.radioButtonLocaImage, &QRadioButton::toggled, this, [this](bool state) {
        if (state) {
            ui.pushButtonOpenLocalImage->setEnabled(true);
            ui.pushButtonUpdataCamera->setEnabled(false);   
            sourceImageType = IMAGESOURCE::SourceLocalImage;
        }
        });
    connect(ui.radioButtonLocaImageDir, &QRadioButton::toggled, this, [this](bool state) {
        if (state) {
            ui.pushButtonOpenLocalImage->setEnabled(true);
            ui.pushButtonUpdataCamera->setEnabled(false);
            sourceImageType = IMAGESOURCE::SourceLocalImageDir;
        }
        });
    connect(ui.radioButtonCamera, &QRadioButton::toggled, this, [this](bool state) {
        if (state) {
            ui.pushButtonOpenLocalImage->setEnabled(false);
            ui.pushButtonUpdataCamera->setEnabled(true);
            sourceImageType = IMAGESOURCE::SourceImageCamera;
        }
        });

    connect(ui.comboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), [this](const QString& str) {
        functionParam.cameraName = str;
        });

    //超时时间
    connect(ui.spinBoxTimeOut, QOverload<int>::of(&QSpinBox::valueChanged),
        this, [this](int value) {
            functionParam.timeOut = value;
        });

    connect(ui.pushButtonUpdataCamera, &QPushButton::clicked, this, [this]() {
        std::vector<QString> v_keys;
        ToolInterface::toolNodeDataInteract->GetMapExDeviceKeys(v_keys);
        if (v_keys.empty()) {
            QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机为空，请先绑定相机");
            return;
        }
        ui.comboBox->clear();
        for (const auto key : v_keys) {
            ui.comboBox->addItem(key);
        }
     });  //按钮按下后信号发出执行操作

    connect(ui.pushButtonOpenLocalImage, &QPushButton::clicked, this, [this]() {
        this->ReadLoalImageAndDir();
    });  //按钮按下后信号发出执行操作

    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [&]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramImageSource::InitialExserializeUi()
{
    connect(this, &FramImageSource::SendExserializeFinishSig, this, [this]() {
        IMAGESOURCE source_type = functionParam.sourceImagType;
        ui.spinBoxTimeOut->setValue(functionParam.timeOut);
        switch (source_type)
        {
        case SourceLocalImage: {
            ui.radioButtonLocaImage->toggled(true);
            ui.radioButtonLocaImage->click();
            ui.qLineEditLocalImagePath->setText(functionParam.sourceImagePath);
        }
            break;
        case SourceLocalImageDir: {
            ui.radioButtonLocaImageDir->toggled(true);
            ui.radioButtonLocaImageDir->click();
            ui.qLineEditLocalImagePath->setText(functionParam.sourceImageDir);     
            ReadImageDirPath(functionParam.sourceImageDir);

        }
            break;
        case SourceImageCamera: {
            ui.radioButtonCamera->toggled(true);
            ui.radioButtonCamera->click();
            ui.comboBox->addItem(functionParam.cameraName);
        }
            break;
        default:
            break;
        }  
        });
}

void FramImageSource::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS:
        if (!ptrVMMat->vmMat.empty()) {
            QImage image = CvMat2QImage(ptrVMMat->vmMat);
            ptrGraphicsViews2d->DispImage(image);
        }
        qstr_des += "运行成功";
        break;
    case RUNNING_FAIL:     qstr_des += "运行失败"; break;
    case DATA_NULL:        qstr_des += "输入数据为空，运行失败"; break;
    case PARAM_EINVAL:     qstr_des += "参数错误，运行失败"; break;
    default:               qstr_des += "未知状态"; break;
    }
    QString str_time = "耗时:" + QString::number(time, 'f', 2) + "ms -- ";
    ui.txtDescribe->setText(str_time + qstr_des);
}


int FramImageSource::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;
    float  elapsed_time = 0.0;
    ptrVMMat->state = false;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_prefix_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_prefix_log + "图像开始采集";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    switch (sourceImageType){

        case SourceLocalImage: { 
           auto qstr_path =  ui.qLineEditLocalImagePath->text();
           ptrVMMat->vmMat = cv::imread(qstr_path.toStdString(), cv::IMREAD_ANYCOLOR);
        }
            break;
        case SourceLocalImageDir: {
            if (vImageDirPaths.empty()) {
                node_state = RUNNINGSTATE::DATA_NULL;
                
                p_log_info.first = LOGTYPE::ERRORS;
                p_log_info.second = str_prefix_log + "文件为空，请检查";
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

                if (isSelfRunring) {
                    emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
                }
                else {
                    emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
                }
                return -1;
            }
            if (numLocalImage >= vImageDirPaths.size()) numLocalImage = 0;
            std::string str_path = vImageDirPaths[numLocalImage];
            ptrVMMat->vmMat = cv::imread(str_path, cv::IMREAD_ANYCOLOR);
            numLocalImage++;   
        }
            break;
        case SourceImageCamera: {
            ptrMapQueuePtrExDeviceData = ToolInterface::toolNodeDataInteract->ptrMapQueuePtrExDeviceData;
            auto iter = ptrMapQueuePtrExDeviceData->find(functionParam.cameraName);
            if (iter == ptrMapQueuePtrExDeviceData->end()) {
                node_state = RUNNINGSTATE::PARAM_EINVAL;
                
                p_log_info.first = LOGTYPE::ERRORS;
                p_log_info.second = str_prefix_log + "相机句柄检索为nullPtr，请检查后重启! ";
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

                if (isSelfRunring) {
                    emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
                }
                else {
                    emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
                }
                return -1;
            }

            std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>> ptr_queue = iter->second;
            ExDeviceData::Ptr ptr_data;
            auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(functionParam.timeOut);
            while (std::chrono::steady_clock::now() < deadline) {
                if (ptr_queue->try_pop(ptr_data)) {
                    if (ptr_data) {
                        break;  // 成功拿到有效数据
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            if (!ptr_data) {
                p_log_info.first = LOGTYPE::ERRORS;
                p_log_info.second = str_prefix_log + " 相机数据等待超时，请检查相机状态! ";
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
                return -1;
            }
            auto ptr_image = std::dynamic_pointer_cast<DeviceCameraData2D>(ptr_data);
            if (!ptr_image) {
                p_log_info.first = LOGTYPE::ERRORS;
                p_log_info.second = str_prefix_log + " 相机数据错误!";
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
                return -1;
            }
            ptrVMMat->vmMat = ptr_image->cameraData.clone();
            ptrVMMat->timeStamp = ptr_image->timeStamp;
        }
            break;
        default:
            break;
    }

    ptrVMMat->state = true;
    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_prefix_log + " 图像获取成功;-- Time: " + QString::number(elapsed_time);
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

int FramImageSource::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramImageSource::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramImageSource::RunningTool, this);
    thrad.detach();
    return 0;
}

QJsonObject FramImageSource::SerializeFunciton()
{
    QJsonObject model_json;
    functionParam.sourceImagType = sourceImageType;
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramImageSource::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    emit SendExserializeFinishSig();
}

void FramImageSource::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrVMMat->nodeID = node_id;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrVMMat);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

void FramImageSource::ReadLoalImageAndDir()
{
    QString path = QDir::currentPath();
    switch (sourceImageType)
    {
    case SourceLocalImage: {
        QString image_path = QFileDialog::getOpenFileName(this, tr("打开图片"), path, "*");
        if (false == image_path.isEmpty()) {
            ui.qLineEditLocalImagePath->setText(image_path);
            functionParam.sourceImagePath = image_path;
        }
    }
        break;
    case SourceLocalImageDir: {
        QString path = QDir::currentPath();
        QString image_dir = QFileDialog::getExistingDirectory(this, tr("打开文件夹"), "D:\\Data",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (image_dir.isEmpty()) {
            return;
        }
        ui.qLineEditLocalImagePath->setText(image_dir);
        functionParam.sourceImageDir = image_dir;  
        ReadImageDirPath(image_dir);
    }
        break;
    case SourceImageCamera:
        break;
    default:
        break;
    }
}

void FramImageSource::ReadImageDirPath(const QString& qstr)
{
    std::vector<std::string> v_str;
    std::string std_str = qstr.toStdString();
    GetAllFiles(std_str, v_str);
    std::vector<std::string>().swap(vImageDirPaths);
    for (auto str : v_str) {
        std::string suffix_str = str.substr(str.find_last_of('.') + 1);
        if (suffix_str == "jpg" || suffix_str == "png" || suffix_str == "bmp")
            vImageDirPaths.push_back(str);
    }
}

void FramImageSource::GetAllFiles(const std::string& path, std::vector<std::string>& files)
{
    long long hFile = 0;
    struct _finddata_t fileinfo;
    std::string p;

    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
                }
            }
            else {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

bool FramImageSource::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::EnterWhatsThisMode) {

        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

