#pragma once
#include <QString>
#include <memory>
#include <mutex>

class FolderOperation
{
public:
	using Ptr = std::shared_ptr<FolderOperation>;
	FolderOperation();
	~FolderOperation();
	//删除N天前的文件；
	bool FindFileForDelete( QString path, int day = -30);

	//删除N天前的文件夹；
	bool FindFolderForDelete(QString path, int day = 30);
};
using PtrFolderOperation = std::shared_ptr<FolderOperation>;

