#pragma once
#include <vector>
#include <opennurbs.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
// 读取控制点（支持逗号分隔）
std::vector<ON_3dPoint> ReadControlPointsFromFile(const std::string& filename)
{
    std::vector<ON_3dPoint> points;
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        // 用 stringstream 解析一行
        std::replace(line.begin(), line.end(), ',', ' '); // 把逗号替换为空格
        std::stringstream ss(line);
        double x, y, z;
        if (ss >> x >> y >> z)
            points.emplace_back(x, y, z);
    }

    infile.close();
    return points;
}