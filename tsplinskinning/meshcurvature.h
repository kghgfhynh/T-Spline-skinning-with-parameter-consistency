#pragma once
#include "Ohmethod.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <cmath>

//向量加减,标量乘法
struct Vec3 {
    double x, y, z;

    Vec3 operator+(const Vec3& b) const { return { x + b.x, y + b.y, z + b.z }; }
    Vec3 operator-(const Vec3& b) const { return { x - b.x, y - b.y, z - b.z }; }
    Vec3 operator*(double s) const { return { x * s, y * s, z * s }; }
};

inline double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline double norm(const Vec3& a) {
    return std::sqrt(dot(a, a));
}
struct Mesh {
    std::vector<Vec3> V;
    std::vector<std::array<int, 3>> F;
};
Mesh loadOBJ(const std::string& filename);

void computeGlobalMeanCurvature_OBJ(const Mesh& mesh);


