#include "FramImageROISelection.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QComboBox>
#include"ImageWithRect.h"
#include <QStandardItemModel>
#include <QHeaderView>

#pragma execution_character_set("utf-8")

FramImageROISelection::FramImageROISelection(QWidget* parent)
    : ToolInterface(parent), m_maxRectIndex(-1)
{
    ui.setupUi(this);
    Initial();
}


FramImageROISelection::~FramImageROISelection()
{}

void FramImageROISelection::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();
    ptrImageROISelectAlgorithm = std::make_shared<ImageROISelectAlgorithm>();
}

void FramImageROISelection::InitialNodeData()
{
    ptrOutVec = std::make_shared<VMUnorderedMap<int, PtrVMVector<VMRectangle>>>();
    ptrOutNodeData = std::make_shared<BaseNodeData>();
    ptrOutNodeData->SetCGObjectNodeFlow(ptrOutVec);

    FlowNodeAndName out_node_and_name;
    out_node_and_name.nodeName = "区域集";
    out_node_and_name.ptrNodedata = ptrOutNodeData;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name));

    //输入
    ptrInNodeVMMat = std::make_shared<BaseNodeData>();
    ptrInNodeVMMat->AddScopeNodeDataType(QtNodes::NodeDataType{ "5","Mat" });

    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "图像";
    in_node_and_name.ptrNodedata = ptrInNodeVMMat;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));

}

void FramImageROISelection::InitialPushButton()
{
    connect(ui.m_exbtn, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  
}

void FramImageROISelection::InitialImageShow()
{
    this->setWindowTitle("区域选择");
    this->setWindowIcon(QIcon(":/FramImageFilter/Image/region.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height()- this->height()) / 2);
    ptrGraphicsViews2d = new QGraphicsViews2d(ui.m_show_widget);
    QRect rect = ui.m_show_widget->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));
    connect(this, &FramImageROISelection::SendNodeRuningStateSyncSig, this, &FramImageROISelection::AcceptSelfRunringStateSlot);
}

void FramImageROISelection::InitilaToolParamUI()
{
    //ui.tabWidgetToolParam->tabBar()->hide();

 // 修改 RectItem 创建时的连接
    connect(ui.m_add_btn, &QPushButton::clicked, this, [=]()
        {
            // 使用下一个可用的索引（从0开始）
            int newIndex = m_maxRectIndex + 1;

            // 确定新的类别ID（使用当前classIds中的最大值+1，或者默认为0）
            int newClassId = 0;
            if (!classIds.empty()) {
                newClassId = *std::max_element(classIds.begin(), classIds.end()) + 1;
            }

            // 创建矩形时传递类别ID
            RectItem* newRect = new RectItem(10, 10, 100, 100, newIndex, newClassId);

            // 更新最大索引
            m_maxRectIndex = newIndex;

            // 设置矩形的标志
            newRect->setFlag(QGraphicsItem::ItemIsSelectable, true);
            newRect->setFlag(QGraphicsItem::ItemIsMovable, true);

            // 连接删除信号
            connect(newRect, &RectItem::requestDelete, this, &FramImageROISelection::onRectItemDeleteRequested);

            ptrGraphicsViews2d->AddItems(newRect);
            m_rectangleItems.append(newRect);
            classIds.push_back(newClassId); // 使用新的类别ID

            // 更新表格显示
            updateTableDisplay();
        });

    // 修改删除按钮逻辑
    connect(ui.m_delete_btn, &QPushButton::clicked, this, [=]()
        {
            // 查找选中的矩形
            bool foundSelected = false;
            for (int i = 0; i < m_rectangleItems.size(); ++i) {
                RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[i]);
                if (rectItem && rectItem->isSelected()) {
                    onRectItemDeleteRequested(i);
                    foundSelected = true;
                    break; // 只删除第一个选中的矩形
                }
            }

            // 如果没有选中的矩形，删除最后一个
            if (!foundSelected && !m_rectangleItems.isEmpty()) {
                int lastIndex = m_rectangleItems.size() - 1;
                onRectItemDeleteRequested(lastIndex);
            }
            else if (!foundSelected) {
                qDebug() << "没有可删除的矩形";
            }
        });

    // 添加场景点击事件来处理空白处点击
    if (ptrGraphicsViews2d->scene) {
        connect(ptrGraphicsViews2d->scene, &QGraphicsScene::selectionChanged, this, [this]() {
            // 当场景选择变化时，同步更新矩形的选中状态
            QList<QGraphicsItem*> selectedItems = ptrGraphicsViews2d->scene->selectedItems();
            for (BaseItem* item : m_rectangleItems) {
                RectItem* rectItem = dynamic_cast<RectItem*>(item);
                if (rectItem) {
                    rectItem->setSelected(selectedItems.contains(rectItem));
                }
            }
            });
    }

    // 清空按钮
    connect(ui.m_clear_btn, &QPushButton::clicked, this, [=]() {
    // 删除所有图形项
    for (BaseItem* item : m_rectangleItems) {
        ptrGraphicsViews2d->scene->removeItem(item);
        delete item;
    }
    m_rectangleItems.clear();
    classIds.clear();
    ptrGraphicsViews2d->ClearObj();
    
    // 重置最大索引
    m_maxRectIndex = -1;
});


    // 修改m_node_pushButton的点击处理函数
    connect(ui.m_node_pushButton, &QPushButton::clicked, this, [this]() {
        ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();

        if (ptrInVMMat == nullptr) {
            QMessageBox::warning(this, "警告", "输入图像数据为空");
            return;
        }

        // 保存当前矩形状态
        saveRectItemsState();

        ptrGraphicsViews2d->ClearObj();
        QImage image = CvMat2QImage(ptrInVMMat->vmMat);
        ptrGraphicsViews2d->DispImage(image);

        // 恢复矩形状态
        restoreRectItemsState();
        });

    // 打开图像按钮
    connect(ui.m_open_pushButton, &QPushButton::clicked, this, [this]() {
        // 弹出文件选择对话框
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "选择图像文件",
            QDir::homePath(),
            "图像文件 (*.png *.jpg *.jpeg *.bmp *.gif)"
        );

        // 如果用户取消了选择，fileName将为空
        if (fileName.isEmpty()) {
            return;
        }

        // 加载图像
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "错误", "无法加载图像文件");
            return;
        }

        // 保存当前矩形状态
        saveRectItemsState();

        ptrGraphicsViews2d->ClearObj();
        ptrGraphicsViews2d->DispImage(pixmap.toImage());

        // 恢复矩形状态
        restoreRectItemsState();
        });

    // 确定按钮 - 更新参数并显示表格
    connect(ui.m_sure_pushButton, &QPushButton::clicked, this, [=]() {
        functionParam.rects.clear();
        functionParam.classIdsStream.clear();

        // 确保类别ID数量与矩形数量一致
        if (classIds.size() != m_rectangleItems.size()) {
            classIds.resize(m_rectangleItems.size(), 0);
        }

        for (int i = 0; i < m_rectangleItems.size(); ++i) {
            RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[i]);
            if (rectItem) {
                QPointF scenePos = rectItem->pos();
                QRectF boundingRect = rectItem->boundingRect();

                functionParam.rects.push_back(cv::Rect(
                    scenePos.x(),
                    scenePos.y(),
                    boundingRect.width(),
                    boundingRect.height()
                ));
                functionParam.classIdsStream.push_back(classIds[i]);
            }
        }
        // 使用统一的表格更新方法
        updateTableDisplay();
        });
}

void FramImageROISelection::InitialExserializeUi()
{
    connect(this, &FramImageROISelection::SendExserializeFinishSig, this, [this]()
        {
            // 清除现有的矩形
            for (BaseItem* item : m_rectangleItems) {
                if (ptrGraphicsViews2d && ptrGraphicsViews2d->scene) {
                    ptrGraphicsViews2d->scene->removeItem(item);
                }
                delete item;
            }
            m_rectangleItems.clear();
            classIds.clear();

            // 确保数据一致性
            size_t minSize = std::min(functionParam.rects.size(),
                std::min(functionParam.classIdsStream.size(),
                    functionParam.rectIndices.size()));

            if (minSize == 0) return;

            // 重新创建矩形项
            int maxIndex = -1;
            for (int i = 0; i < minSize; i++) {
                const cv::Rect& rect = functionParam.rects[i];
                int classId = functionParam.classIdsStream[i];
                int rectIndex = functionParam.rectIndices[i];

                // 创建矩形时传递类别ID
                RectItem* newRect = new RectItem(
                    rect.x,
                    rect.y,
                    rect.width,
                    rect.height,
                    rectIndex,
                    classId  // 传递类别ID
                );

                // 连接信号
                connect(newRect, &RectItem::requestDelete, this, &FramImageROISelection::onRectItemDeleteRequested);

                ptrGraphicsViews2d->AddItems(newRect);
                m_rectangleItems.append(newRect);
                classIds.push_back(classId);

                if (rectIndex > maxIndex) {
                    maxIndex = rectIndex;
                }
            }
            // 设置最大索引
            m_maxRectIndex = maxIndex;
            // 更新表格显示
            updateTableDisplay();
            ui.inputModelLabels->setText(inputModelLabels.join(","));
        });
}

int FramImageROISelection::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;
    float  elapsed_time = 0.0;

    ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();  
    ptrOutVec->state = false;

    if (ptrInVMMat == nullptr || ptrInVMMat->vmMat.data == nullptr|| ptrInVMMat->state==false) {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);//显示节点运行成功。。。。
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    // 准备位置ID和类别ID列表
    std::vector<int> positionIds;
    std::vector<int> currentClassIds;

    for (int i = 0; i < m_rectangleItems.size(); ++i) {
        RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[i]);
        if (rectItem) {
            positionIds.push_back(rectItem->getIndex()+1);
            // 直接从RectItem获取类别ID
            currentClassIds.push_back(rectItem->getClassId());
        }
    }
    int res = ptrImageROISelectAlgorithm->RunImageRoi(m_rectangleItems, currentClassIds, positionIds, ptrOutVec, functionParam, useInputModelLabels);

    if (res != 0){
        RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_FAIL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return 0;
    }
    ptrOutVec->state = true;

    // 修复：执行后不需要重新创建矩形，直接使用现有的
    if (isSelfRunring) {
        ptrGraphicsViews2d->ClearObj();
        QImage image = CvMat2QImage(ptrInVMMat->vmMat);
        ptrGraphicsViews2d->DispImage(image);

        // 重新添加当前所有矩形项
        for (BaseItem* item : m_rectangleItems) {
            ptrGraphicsViews2d->AddItems(item);
        }
    }

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}



//接受绑定完成的槽函数，用于显示
void FramImageROISelection::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
    if (iter == mapCGObjectFlowInNodeAndName.end())
        return;
    auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
    auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
    QString name = vm_object->nodeDataType.name;
    QtNodes::NodeId node_id = ptr->GetNodeId();
    ui.m_In_lineEdit->setText(QString::number(node_id) + ": " + name);   
}


void FramImageROISelection::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        // 修复：不需要重新创建矩形，直接刷新显示即可
        ptrGraphicsViews2d->ClearObj();
        QImage image = CvMat2QImage(ptrInVMMat->vmMat);
        ptrGraphicsViews2d->DispImage(image);

        // 重新添加当前所有矩形项
        for (BaseItem* item : m_rectangleItems) {
            ptrGraphicsViews2d->AddItems(item);
        }

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
    ui.m_lineEdit->setText(str_time + qstr_des);
   
}

// 新增：更新表格显示的方法
void FramImageROISelection::updateTableDisplay()
{
    QStandardItemModel* model = new QStandardItemModel(m_rectangleItems.size(), 3, ui.m_tableView);
    model->setHorizontalHeaderLabels({ "序号", "动作顺序", "类别ID" });

    for (int row = 0; row < m_rectangleItems.size(); ++row) {
        RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[row]);

        // 列表序号（从1开始）
        QStandardItem* listIndexItem = new QStandardItem(QString::number(row + 1));
        listIndexItem->setEditable(false);
        listIndexItem->setTextAlignment(Qt::AlignCenter);
        model->setItem(row, 0, listIndexItem);

        // 矩形序号（位置ID）- 设置为可编辑（从1开始显示）
        QStandardItem* rectIndexItem = new QStandardItem(QString::number(rectItem->getIndex() + 1));
        rectIndexItem->setEditable(true);
        rectIndexItem->setTextAlignment(Qt::AlignCenter);
        rectIndexItem->setData(row, Qt::UserRole);
        model->setItem(row, 1, rectIndexItem);

        // 类别ID - 从RectItem获取
        QStandardItem* classIdItem = new QStandardItem(QString::number(rectItem->getClassId()));
        classIdItem->setEditable(true);
        classIdItem->setTextAlignment(Qt::AlignCenter);
        classIdItem->setData(row, Qt::UserRole + 1);
        model->setItem(row, 2, classIdItem);

        // 确保classIds数组与当前矩形项同步
        if (row < classIds.size()) {
            classIds[row] = rectItem->getClassId();
        }
        else {
            classIds.push_back(rectItem->getClassId());
        }
    }

    ui.m_tableView->setModel(model);
    ui.m_tableView->verticalHeader()->hide();
    ui.m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 断开之前的连接，避免重复连接
    disconnect(model, &QStandardItemModel::itemChanged, this, &FramImageROISelection::onTableItemChanged);
    connect(model, &QStandardItemModel::itemChanged, this, &FramImageROISelection::onTableItemChanged);
}


void FramImageROISelection::onTableItemChanged(QStandardItem* item)
{
    if (item == nullptr) return;
    int row = item->row();
    int column = item->column();

    if (row < 0 || row >= m_rectangleItems.size()) {
        return;
    }

    RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[row]);
    if (!rectItem) return;

    if (column == 1) { // 动作顺序列
        int newPositionId = item->text().toInt() - 1;
        rectItem->setIndex(newPositionId);
        if (newPositionId > m_maxRectIndex) {
            m_maxRectIndex = newPositionId;
        }
    }
    else if (column == 2) { // 类别ID列
        int newClassId = item->text().toInt();
        // 更新RectItem的类别ID
        rectItem->setClassId(newClassId);
        // 更新classIds数组
        if (row < classIds.size()) {
            classIds[row] = newClassId;
        }
        else {
            classIds.push_back(newClassId);
        }
    }
}

// 新增槽函数处理位置ID的修改
void FramImageROISelection::onPositionIdChanged(QStandardItem* item)
{
    if (item->column() == 1) { // 只处理位置ID列
        int row = item->row();
        int newPositionId = item->text().toInt();

        // 确保行索引有效
        if (row >= 0 && row < m_rectangleItems.size()) {
            RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[row]);
            if (rectItem) {
                // 更新矩形的索引
                rectItem->setIndex(newPositionId);

                // 更新最大索引（如果需要）
                if (newPositionId > m_maxRectIndex) {
                    m_maxRectIndex = newPositionId;
                }
                // 可选：刷新显示以确保一致性
                updateTableDisplay();
            }
        }
    }
}

// 实现删除指定矩形的槽函数
void FramImageROISelection::onRectItemDeleteRequested(int index)
{
    if (index < 0 || index >= m_rectangleItems.size()) {
        //qDebug() << "删除索引无效:" << index;
        return;
    }
    // 获取要删除的矩形序号
    RectItem* itemToDelete = dynamic_cast<RectItem*>(m_rectangleItems[index]);
    int rectIndexToDelete = itemToDelete->getIndex();

    // 删除图形项
    if (ptrGraphicsViews2d && ptrGraphicsViews2d->scene) {
        ptrGraphicsViews2d->scene->removeItem(itemToDelete);
    }
    delete itemToDelete;
    // 从列表中移除
    m_rectangleItems.removeAt(index);
    // 删除对应的类别ID
    if (index < classIds.size()) {
        classIds.erase(classIds.begin() + index);
    }
    // 更新表格显示，保持正确的映射关系
    updateTableDisplay();
}

// 添加事件过滤器处理矩形项的点击选中
bool FramImageROISelection::eventFilter(QObject* watched, QEvent* event)
{
    // 先调用基类的 eventFilter
    if (ToolInterface::eventFilter(watched, event)) {
        return true;
    }

    if (event->type() == QEvent::GraphicsSceneMousePress && watched == ptrGraphicsViews2d->scene) {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        // 检查是否点击了矩形项
        QGraphicsItem* item = ptrGraphicsViews2d->scene->itemAt(mouseEvent->scenePos(), QTransform());
        RectItem* rectItem = dynamic_cast<RectItem*>(item);

        if (rectItem) {
            // 清除所有矩形的选中状态
            for (BaseItem* baseItem : m_rectangleItems) {
                RectItem* currentRect = dynamic_cast<RectItem*>(baseItem);
                if (currentRect) {
                    currentRect->setSelected(false);
                }
            }
            // 设置当前矩形的选中状态
            rectItem->setSelected(true);
            return true; // 事件已处理
        }
    }
    return false; // 事件未处理，继续传递
}

// 清除所有选中状态
void FramImageROISelection::clearAllSelections()
{
    for (BaseItem* baseItem : m_rectangleItems) {
        RectItem* rectItem = dynamic_cast<RectItem*>(baseItem);
        if (rectItem) {
            rectItem->setSelected(false);
        }
    }
}


// 保存矩形状态
void FramImageROISelection::saveRectItemsState()
{
    m_savedRects.clear();
    m_savedClassIds.clear();
    m_savedIndices.clear();

    for (int i = 0; i < m_rectangleItems.size(); ++i) {
        RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[i]);
        if (rectItem) {
            QPointF scenePos = rectItem->pos();
            QRectF boundingRect = rectItem->boundingRect();

            m_savedRects.append(QRectF(scenePos.x(), scenePos.y(),
                boundingRect.width(), boundingRect.height()));
            m_savedClassIds.append(rectItem->getClassId());
            m_savedIndices.append(rectItem->getIndex());
        }
    }
}

// 恢复矩形状态
void FramImageROISelection::restoreRectItemsState()
{
    // 清除现有矩形
    for (BaseItem* item : m_rectangleItems) {
        if (ptrGraphicsViews2d && ptrGraphicsViews2d->scene) {
            ptrGraphicsViews2d->scene->removeItem(item);
        }
        delete item;
    }
    m_rectangleItems.clear();
    classIds.clear();

    // 重新创建保存的矩形
    for (int i = 0; i < m_savedRects.size(); ++i) {
        QRectF rect = m_savedRects[i];
        int classId = m_savedClassIds[i];
        int index = m_savedIndices[i];

        RectItem* newRect = new RectItem(rect.x(), rect.y(),
            rect.width(), rect.height(),
            index, classId);

        newRect->setFlag(QGraphicsItem::ItemIsSelectable, true);
        newRect->setFlag(QGraphicsItem::ItemIsMovable, true);
        connect(newRect, &RectItem::requestDelete, this, &FramImageROISelection::onRectItemDeleteRequested);

        ptrGraphicsViews2d->AddItems(newRect);
        m_rectangleItems.append(newRect);
        classIds.push_back(classId);
    }

    // 更新最大索引
    if (!m_savedIndices.isEmpty()) {
        m_maxRectIndex = *std::max_element(m_savedIndices.begin(), m_savedIndices.end());
    }
    else {
        m_maxRectIndex = -1;
    }

    updateTableDisplay();
}

void FramImageROISelection::PushButtonModelParaConfirmClicked()
{
    inputModelLabels = ui.inputModelLabels->text().trimmed().split(',', Qt::SkipEmptyParts);
    useInputModelLabels = ui.checkBoxResultShow->isChecked();
}

int FramImageROISelection::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramImageROISelection::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramImageROISelection::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramImageROISelection::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOutVec);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}


QJsonObject FramImageROISelection::SerializeFunciton()
{
    functionParam.rects.clear();
    functionParam.classIdsStream.clear();
    functionParam.rectIndices.clear();

    // 确保类别ID数量与矩形数量一致
    if (classIds.size() != m_rectangleItems.size()) {
        classIds.resize(m_rectangleItems.size(), 0); // 默认值为0
    }

    for (int i = 0; i < m_rectangleItems.size(); ++i) {
        RectItem* rectItem = dynamic_cast<RectItem*>(m_rectangleItems[i]);
        if (rectItem) {
            QPointF sceneTopLeft = rectItem->mapToScene(rectItem->boundingRect().topLeft());

            functionParam.rects.push_back(cv::Rect(
                static_cast<int>(sceneTopLeft.x()),
                static_cast<int>(sceneTopLeft.y()),
                static_cast<int>(rectItem->boundingRect().width()),
                static_cast<int>(rectItem->boundingRect().height())
            ));

            // 使用当前的classIds（从0开始）
            functionParam.classIdsStream.push_back(classIds[i]);
            functionParam.rectIndices.push_back(rectItem->getIndex());
        }
    }

    functionParam.inputModelLabels = inputModelLabels;

    QJsonObject model_json;
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramImageROISelection::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    ExserializeTFunciton(fun_json, functionParam); 

    inputModelLabels = functionParam.inputModelLabels;

    emit SendExserializeFinishSig();
}

