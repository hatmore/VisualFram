#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string>
#include <io.h>
#include <fstream> 
#include <random>
#include <opencv2/opencv.hpp>

static void read_files_in_dir(const std::string& path, std::vector<std::string>& files)
{
    long long hFile = 0;
    struct _finddata_t fileinfo;
    std::string p;

    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    read_files_in_dir(p.assign(path).append("\\").append(fileinfo.name), files);
                }
            }
            else {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}


static inline int get_random_int(int minThres=0, int maxThres=255){
    // 获取处于某一范围内的一个随机整数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(minThres, maxThres);

    int random_integer = distrib(gen);

    return random_integer;
}

#endif  // UTILS_H
