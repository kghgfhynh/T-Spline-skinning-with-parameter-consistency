#pragma once
#include "opennurbs.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "Ohmethod.h"
struct BasisResult {
    std::vector<double> N;    // 0阶基函数
    std::vector<double> dN;   // 一阶导
    std::vector<double> ddN;  // 二阶导
};

//--------------------------------------
// 用 OpenNURBS 函数计算基函数及导数
//--------------------------------------
BasisResult computeBasisAndDerivatives_ON(
    int degree,
    const std::vector<double>& knots,
    double t);


//--------------------------------------
// 曲面导数结构体
//--------------------------------------
struct SurfaceDerivatives {
    ON_3dPoint  S;
    ON_3dVector Su, Sv;
    ON_3dVector Suu, Suv, Svv;
};

//计算曲面基函数各阶导数
SurfaceDerivatives computeSurfaceDerivatives_Tspline(
    double u, double v,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v);

////计算曲面均匀性泛函
//double computeUniformityFunctional_Gauss5(
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& curves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v
//);
//
////计算曲面离心率泛函
//double computeEccentricityFunctional_Gauss5(
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& curves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v
//);


// 返回结构体（你可以按需改名或把字段合并到已有结构）
struct CurvatureResult {
    double H ;    // mean curvature
    ON_3dVector normal;
    bool valid ; // false if surface is degenerate at the sample
};


struct SamplePointmeancurvature {
    double u, v;
    SurfaceDerivatives der;  // S, Su, Sv, ...
    CurvatureResult curv;    // H, K, k1, k2, normal
    bool valid = false;
};

// 计算一个点的平均曲率
CurvatureResult computeCurvaturesFromDerivatives(const SurfaceDerivatives& res);

//计算曲面每个点的平均曲率
std::vector<std::vector<SamplePointmeancurvature>>
sampleSurfaceCurvatures(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max);

//得到曲面的最大和最小平均曲率
void computeGlobalMeanCurvature(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max);

