#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include "winsock2.h"

class QLabel;
class QTimer;
class QProcess;

class ShowCpuMemory : public QObject
{
    Q_OBJECT

public:
    explicit ShowCpuMemory(QObject* parent = 0);

    void SetLab(QLabel* labCPUMemory);  
    const void StopThread() {
        this->stop = false;
    }

private:
    int memoryPercent;
    int memoryAll;
    int memoryUse;
    int memoryFree;
    QLabel* labMemory;   //显示内存信息的控件
    QProcess* process;
    int cpu;
    bool stop{false};

public slots:
    void GetCpuMemory();

private slots:
    int calCpuUsage();
    //时间转换
    __int64 Filetime2Int64(const FILETIME* ftime);
    //两个时间相减运算
    __int64 CompareFileTime(FILETIME preTime, FILETIME nowTime);
};

