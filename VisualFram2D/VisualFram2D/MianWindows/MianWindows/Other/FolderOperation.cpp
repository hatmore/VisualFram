#include "FolderOperation.h"
#include <QDateTime>
#include <QtWidgets/QWidget>
#include <sstream>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QMessageBox>

FolderOperation::FolderOperation()
{
}

FolderOperation::~FolderOperation()
{

}

//删除N天前的文件
bool FolderOperation::FindFileForDelete(QString path, int day)
{
	QDateTime now = QDateTime::currentDateTime();
	QDateTime dateTime1 = now.addDays(day);
	QDateTime dateTime2;
	QDir dir(path);
	QFileInfoList fileList = dir.entryInfoList();
	for (QFileInfo f : fileList){
		if (f.baseName() == ""){
			continue;
		}
		dateTime2 = f.birthTime();
		if (dateTime2 < dateTime1){
				dir.remove(f.absoluteFilePath());
		}
	}
	return true;
}

//删除N天前的文件夹
bool FolderOperation::FindFolderForDelete(QString path, int day)
{
	QList<QString> dirs;
	dirs.reserve(1000);
	dirs.clear();
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	for (auto fullDir : dir.entryInfoList()){
		if (fullDir.fileName() == "." || fullDir.fileName() == "..")
			continue;
		dirs.push_back(fullDir.absoluteFilePath());
	}
	if (dirs.size() < day){
		return false;
	}
	else {
		QList<quint64> times = QList<quint64>();
		times.reserve(1000);
		times.clear();
		QList<QString> folder = QList<QString>();
		folder.reserve(1000);
		folder.clear();
		for (auto dir : dirs){
			QFileInfo fl(dir);
			QDateTime DT = fl.birthTime();
			QDateTime dt0(QDate(1970, 1, 1));
			quint64 time = DT.toTime_t() - dt0.toTime_t();
			times.append(time);
			folder.append(dir);
		}
		//获取数组中最小值的索引
		int index = -1;
		QVector<quint64> max_and_min(2);
		if (times.size() != 0){
			max_and_min[1] = times[0];
			index = 0;
			for (int i = 0; i < times.size(); i++){
					if (max_and_min[1] > times[i]){
						index = i;
						max_and_min[1] = times[i];
					}
				}
			}
			QDir dir_x;
			if (dir_x.exists(folder[index])){
				dir_x.setPath(folder[index]);
				dir_x.removeRecursively();
		   }
		return true;
	}
}
