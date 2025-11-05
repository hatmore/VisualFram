#include "FramImageMaskFilter.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QBuffer>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

FramImageMaskFilter::FramImageMaskFilter(QWidget* parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramImageMaskFilter::~FramImageMaskFilter()
{}

void FramImageMaskFilter::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();
}

void FramImageMaskFilter::InitialNodeData()
{
    ptrOutVMMat = std::make_shared<VMMat>();
    ptrOutNodeVMMat = std::make_shared<BaseNodeData>();
    ptrOutNodeVMMat->SetCGObjectNodeFlow(ptrOutVMMat);

    FlowNodeAndName out_node_and_name;
    out_node_and_name.nodeName = "图像";
    out_node_and_name.ptrNodedata = ptrOutNodeVMMat;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name));

    ptrInVMMat = std::make_shared<VMMat>();
    ptrInNodeVMMat = std::make_shared<BaseNodeData>();
    //ptrInNodeVMMat->SetCGObjectNodeFlow(ptrInVMMat);
    ptrInNodeVMMat->AddScopeNodeDataType(QtNodes::NodeDataType{ "5","Mat" });

    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "图像";
    in_node_and_name.ptrNodedata = ptrInNodeVMMat;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));

}

void FramImageMaskFilter::InitialPushButton()
{
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
    // 加载节点图像
    connect(ui.pushButtonLoadNodeImage, &QPushButton::clicked, this, [this]() {
        ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();
        displayMat = ptrInVMMat->vmMat;
        QImage image = CvMat2QImage(displayMat);
        ptrGraphicsViews2d->DispImage(image);
        });
    // 加载本地图像
    connect(ui.pushButtonLoadLocalImage, &QPushButton::clicked, this, [this]() {
        QString path = QDir::currentPath();
        QString image_path = QFileDialog::getOpenFileName(this, tr("打开图片"), path, "*");
        if (false == image_path.isEmpty()) {
            ui.qLineEditLocalImagePath->setText(image_path);
        }
        displayMat = cv::imread(image_path.toStdString(), cv::IMREAD_ANYCOLOR);
        QImage image = CvMat2QImage(displayMat);
        ptrGraphicsViews2d->DispImage(image);
        });
    // 添加 ROI
    connect(ui.pushButtonAddROI, &QPushButton::clicked, this, [this]() {
        // 获取场景的中心点
        QRectF sceneRect = ptrGraphicsViews2d->sceneRect();
        qreal centerX = sceneRect.x() + sceneRect.width() / 2.0;
        qreal centerY = sceneRect.y() + sceneRect.height() / 2.0;

        BaseItem* newItem = nullptr;
        ROIType type = static_cast<ROIType>(ui.comboBoxROISelect->currentIndex());
        switch (type) {
        case ROIType::Rectangle: {
            // 矩形的中心为 (centerX, centerY)
            qreal width = 500;
            qreal height = 200;
            /*qreal x = centerX - width / 2.0;
            qreal y = centerY - height / 2.0;*/
            qreal x = centerX;
            qreal y = centerY;
            newItem = new RectangleItem(x, y, width, height);
            break;
        }
        case ROIType::RotatedRectangle: {
            // 旋转矩形的中心为 (centerX, centerY)
            qreal width = 400;
            qreal height = 200;
            qreal x = centerX + 200;
            qreal y = centerY + 200;
            newItem = new RectangleRItem(x, y, width, height, 30);
            break;
        }
        case ROIType::Circle: {
            // 圆的中心为 (centerX, centerY)
            newItem = new CircleItem(centerX + 200, centerY + 200, 200);
            break;
        }
        case ROIType::Polygon: {
            ptrGraphicsViews2d->scene->StartCreate();
            polygon_item = new PolygonItem();
            newItem = polygon_item;
            connect(polygon_item, &PolygonItem::pushPoint, polygon_item, &PolygonItem::pushPoint);
            break;
        }
        default:
            return;
        }

        if (newItem) {
            ptrGraphicsViews2d->AddItems(newItem);
            roiItems.push_back(newItem);
            newItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
            newItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            newItem->setFlag(QGraphicsItem::ItemIsFocusable, true);
        }
        });
    // 删除选中 ROI
    connect(ui.pushButtonDeleteROI, &QPushButton::clicked, this, [this]() {
        if (selectedROIIndex < 0 || selectedROIIndex >= static_cast<int>(roiItems.size())) {
            QMessageBox::warning(this, "提示", "请先选择一个 ROI！");
            return;
        }

        int deleteIndex = selectedROIIndex;
        BaseItem* item = roiItems[deleteIndex];
        ptrGraphicsViews2d->scene->removeItem(item);
        roiItems.erase(roiItems.begin() + deleteIndex);
        delete item;
        selectedROIIndex = -1;
        if (item == polygon_item) {
            polygon_item = nullptr;
        }
        });
    // 选择 ROI 类型
    connect(ui.comboBoxROISelect, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        selectedROIIndex = index; // 更新选择的 ROI 类型
        });
    // 清空所有 ROI
    connect(ui.pushButtonClearAllROIs, &QPushButton::clicked, this, &FramImageMaskFilter::PushButtonClearAllROIsClicked);
}

void FramImageMaskFilter::InitialImageShow()
{
    this->setWindowTitle("图像掩膜");
    this->setWindowIcon(QIcon(":/FramImageMaskFilter/Image/ImageMaskFilter.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);


    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    ptrGraphicsViews2d->setObjectName(QString::fromUtf8("ptrGraphicsViews2d"));
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    // 连接场景的选择变化信号以处理 ROI 选择
    connect(ptrGraphicsViews2d->scene, &QGraphicsScene::selectionChanged, this, [this]() {
        auto selectedItems = ptrGraphicsViews2d->scene->selectedItems();
        selectedROIIndex = -1;
        if (!selectedItems.isEmpty()) {
            QGraphicsItem* selected = selectedItems.first();
            for (size_t i = 0; i < roiItems.size(); ++i) {
                if (roiItems[i] == selected) {
                    selectedROIIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    });
    connect(this, &FramImageMaskFilter::SendNodeRuningStateSyncSig, this, &FramImageMaskFilter::AcceptSelfRunringStateSlot);
}

void FramImageMaskFilter::InitilaToolParamUI()
{
    QStringList items = { tr("矩形"), tr("旋转矩形"), tr("圆形"), tr("多边形") };
    ui.comboBoxROISelect->addItems(items);
    ui.comboBoxROISelect->setCurrentIndex(static_cast<int>(ROIType::Polygon));
}

void FramImageMaskFilter::InitialExserializeUi()
{

}


int FramImageMaskFilter::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "判断输入数据状态";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();
    ptrOutVMMat->state = false;

    if (ptrInVMMat == nullptr || ptrInVMMat->vmMat.data == nullptr) {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;

        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_log + "节点输入数据为空，请检查！";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }
    //掩膜处理
    cv::Mat mask = cv::Mat::zeros(ptrInVMMat->vmMat.size(), CV_8UC1);
    // 遍历所有 ROI，生成掩膜
    for (const auto& item : roiItems) {
        switch (item->GetType()) {
        case BaseItem::Rectangle: {
            auto rectItem = dynamic_cast<RectangleItem*>(item);
            MRectangle MRect;
            rectItem->GetRect(MRect);
            cv::Rect rect(static_cast<int>(MRect.x), static_cast<int>(MRect.y),
                static_cast<int>(MRect.width), static_cast<int>(MRect.height));
            cv::rectangle(mask, rect, cv::Scalar(255), -1); // 填充矩形区域
            break;
        }
        case BaseItem::RectangleR: {
            auto rRectItem = dynamic_cast<RectangleRItem*>(item);
            MRotatedRect MRRect;
            rRectItem->GetRotatedRect(MRRect);
            cv::Point2f center(MRRect.x + MRRect.width / 2.0f, MRRect.y + MRRect.height / 2.0f);
            cv::RotatedRect rotatedRect(center, cv::Size2f(MRRect.width, MRRect.height), MRRect.phi);
            cv::Point2f vertices[4];
            rotatedRect.points(vertices);
            std::vector<cv::Point> contour;
            for (int i = 0; i < 4; ++i) {
                contour.push_back(cv::Point(static_cast<int>(vertices[i].x), static_cast<int>(vertices[i].y)));
            }
            cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{contour}, cv::Scalar(255));
            break;
        }
        case BaseItem::Circle: {
            auto circleItem = dynamic_cast<CircleItem*>(item);
            MCircle cir;
            circleItem->GetCircle(cir);
            cv::circle(mask, cv::Point(static_cast<int>(cir.x), static_cast<int>(cir.y)),
                static_cast<int>(cir.radius), cv::Scalar(255), -1); // 填充圆形区域
            break;
        }
        case BaseItem::Polygon: {
            auto polygonItem = dynamic_cast<PolygonItem*>(item);
            MPolygon MPolygon;
            polygonItem->GetPolygon(MPolygon);
            std::vector<cv::Point> contour;
            for (const auto& pt : MPolygon.listPoint) {
                contour.emplace_back(static_cast<int>(pt.x()), static_cast<int>(pt.y()));
            }
            if (!contour.empty()) {
                cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{contour}, cv::Scalar(255));
            }
            break;
        }
        default:
            break;
        }
    }
    ptrOutVMMat->vmMat = cv::Mat::zeros(ptrInVMMat->vmMat.size(), ptrInVMMat->vmMat.type());
    cv::bitwise_and(ptrInVMMat->vmMat, ptrInVMMat->vmMat, ptrOutVMMat->vmMat, mask);
    ptrOutVMMat->state = true;

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + " 图形掩膜完成;-- Time: " + QString::number(elapsed_time);
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

void FramImageMaskFilter::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
    if (iter == mapCGObjectFlowInNodeAndName.end())
        return;

    auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
    auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
    QString name = vm_object->nodeDataType.name;
    QtNodes::NodeId node_id = ptr->GetNodeId();
    ui.qLineEditLocalImagePath->setText(QString::number(node_id) + ": " + name);
}


void FramImageMaskFilter::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    displayMat = ptrOutVMMat->vmMat;
    //获取掩膜输出图像
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        QImage image = CvMat2QImage(displayMat);
        ptrGraphicsViews2d->DispImage(image);
        qstr_des = qstr_des + "运行成功";
    }
                       break;
    case RUNNING_FAIL: {
        qstr_des = qstr_des + "运行失败";
    }
                     break;
    case DATA_NULL: {
        qstr_des = qstr_des + "输入数据为空，运行失败";
    }
                  break;
    case PARAM_EINVAL: {
        qstr_des = qstr_des + "参数错误，运行失败";
    }
                     break;
    default:
        break;
    }
    QString str_time = "耗时:" + QString::number(time, 'f', 2) + "ms -- ";
    ui.txtDescribe->setText(str_time + qstr_des);
}

void FramImageMaskFilter::PushButtonClearAllROIsClicked()
{
    for (auto item : roiItems) {
        ptrGraphicsViews2d->scene->removeItem(item);
        delete item;
    }
    roiItems.clear();
    selectedROIIndex = -1;
    polygon_item = nullptr;
}

int FramImageMaskFilter::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramImageMaskFilter::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramImageMaskFilter::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramImageMaskFilter::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrOutVMMat->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOutVMMat);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramImageMaskFilter::SerializeFunciton()
{
    QJsonObject model_json;

    //functionParam.filterMethod = filterMethod;
    //functionParam.meanBlurParam = meanBlurParam;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);

    //序列化 ROI
    QJsonArray roiArray;
    for (const auto* item : roiItems) {
        QJsonObject roiObj;
        switch (item->GetType()) {
        case BaseItem::Rectangle: {
            auto rectItem = dynamic_cast<const RectangleItem*>(item);
            MRectangle rect;
            rectItem->GetRect(rect);
            roiObj["type"] = "Rectangle";
            roiObj["x"] = rect.x;
            roiObj["y"] = rect.y;
            roiObj["width"] = rect.width;
            roiObj["height"] = rect.height;
            break;
        }
        case BaseItem::RectangleR: {
            auto rRectItem = dynamic_cast<const RectangleRItem*>(item);
            MRotatedRect rrect;
            rRectItem->GetRotatedRect(rrect);
            roiObj["type"] = "RectangleR";
            roiObj["x"] = rrect.x;
            roiObj["y"] = rrect.y;
            roiObj["width"] = rrect.width;
            roiObj["height"] = rrect.height;
            roiObj["angle"] = rrect.phi;
            break;
        }
        case BaseItem::Circle: {
            auto circleItem = dynamic_cast<const CircleItem*>(item);
            MCircle cir;
            circleItem->GetCircle(cir);
            roiObj["type"] = "Circle";
            roiObj["x"] = cir.x;
            roiObj["y"] = cir.y;
            roiObj["radius"] = cir.radius;
            break;
        }
        case BaseItem::Polygon: {
            auto polygonItem = dynamic_cast<const PolygonItem*>(item);
            MPolygon poly;
            polygonItem->GetPolygon(poly);
            roiObj["type"] = "Polygon";
            QJsonArray pointsArray;
            for (const auto& pt : poly.listPoint) {
                QJsonArray point;
                point.append(pt.x());
                point.append(pt.y());
                pointsArray.append(point);
            }
            roiObj["points"] = pointsArray;
            break;
        }
        default:
            continue;
        }
        roiArray.append(roiObj);
    }
    model_json["ROIs"] = roiArray;

    return model_json;
}

void FramImageMaskFilter::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    //filterMethod = functionParam.filterMethod;
    //meanBlurParam = functionParam.meanBlurParam;

    //反序列化 ROI
    PushButtonClearAllROIsClicked();  // 安全清空

    if (json.contains("ROIs") && json["ROIs"].isArray()) {
        QJsonArray roiArray = json["ROIs"].toArray();
        for (const auto& v : roiArray) {
            QJsonObject roiObj = v.toObject();
            QString type = roiObj["type"].toString();
            BaseItem* newItem = nullptr;

            if (type == "Rectangle") {
                double x = roiObj["x"].toDouble();
                double y = roiObj["y"].toDouble();
                double w = roiObj["width"].toDouble();
                double h = roiObj["height"].toDouble();
                newItem = new RectangleItem(x, y, w, h);
            }
            else if (type == "RectangleR") {
                double x = roiObj["x"].toDouble();
                double y = roiObj["y"].toDouble();
                double w = roiObj["width"].toDouble();
                double h = roiObj["height"].toDouble();
                double angle = roiObj["angle"].toDouble();
                newItem = new RectangleRItem(x, y, w, h, angle);
            }
            else if (type == "Circle") {
                double x = roiObj["x"].toDouble();
                double y = roiObj["y"].toDouble();
                double r = roiObj["radius"].toDouble();
                newItem = new CircleItem(x, y, r);
            }
            else if (type == "Polygon") {
                QJsonArray pointsArray = roiObj["points"].toArray();
                QList<QPointF> pts;
                for (const auto& pv : pointsArray) {
                    QJsonArray point = pv.toArray();
                    if (point.size() >= 2) {
                        pts.append(QPointF(point[0].toDouble(), point[1].toDouble()));
                    }
                }

                // 直接用有参构造函数创建
                PolygonItem* polyItem = new PolygonItem(pts);  // 内部自动创建 ControlItem
                polyItem->FinishCreate();  // 确保可编辑
                newItem = polyItem;
            }

            if (newItem) {
                ptrGraphicsViews2d->AddItems(newItem);
                roiItems.push_back(newItem);
                newItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
                newItem->setFlag(QGraphicsItem::ItemIsMovable, true);
                newItem->setFlag(QGraphicsItem::ItemIsFocusable, true);
            }
        }
    }

    emit SendExserializeFinishSig();
}

