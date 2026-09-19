#pragma once
#define _USE_MATH_DEFINES
#include <opennurbs.h>
#include "Ohmethod.h"
#include "Penaltyfunc.h"
#include "paramatch.h"

#include <vector>
#include <iostream>
// ------------------------------------------------------------
// 计算单条曲线在 [v1, v2] 的弧长
// ------------------------------------------------------------
double ComputeLocalArcLength(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    int samples=200
);
double FindParameterByArcLength(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    double targetLength,
    int samples=200
);
std::vector<double> ComputeGeometricUniformV_Local(
    const std::vector<ON_NurbsCurve>& guideCurves,
    double v1,
    double v2,
    int insertCount
);
// 输入：每条曲线的等参数采样点
// 输出：每条形状引导线的采样点集合
//转置
std::vector<std::vector<ON_3dPoint>> Transpose(
    const std::vector<std::vector<ON_3dPoint>>& samplestoguide);

//增加端点导数要求后求解控制点
void SolveControlPoints_1Der(
    int degree,                                       // 曲线次数-1，即degree=p
    const std::vector<double>& knotVector,           // 该NURBS的节点向量（非重复端点）
    const std::vector<double>& params,                // 参数点(u_0,u_1,...),每个对应一个空间点
    const std::vector<ON_3dPoint>& points,            // 空间点序列，对应参数点的曲线点坐标Y
    std::vector<ON_3dPoint>& controlPoints            // 输出的控制点序列Q，反求得
);
void SolveControlPoints_2Der(
    int degree,                                       // 曲线次数-1，即degree=p
    const std::vector<double>& knotVector,           // 该NURBS的节点向量（非重复端点）
    const std::vector<double>& params,                // 参数点(u_0,u_1,...),每个对应一个空间点
    const std::vector<ON_3dPoint>& points,            // 空间点序列，对应参数点的曲线点坐标Y
    std::vector<ON_3dPoint>& controlPoints            // 输出的控制点序列Q，反求得
);

//引导线插值
std::vector<ON_NurbsCurve> GenerateShapeGuides(
    std::vector<std::vector<ON_3dPoint>>& guidelineSamples,//引导线上的点
    int degree,
    int dim,
    int order,
    const std::string& debugObjName,
    std::vector<std::vector<double>>& allGuideParams);

//// 生成 T 样条曲面采样点（二维顶点网格）
//std::vector<std::vector<ON_3dPoint>>
//SampleTSplineSurface(
//    int u_samples,//allguidecurves.size()
//    int v_samples,//shape_point_num
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& ALLREcurves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v,
//    double u_min,
//    double u_max,
//    double v_min,
//    double v_max
//);
//
////计算曲面点与形状点间的距离
//std::vector<std::vector<ON_3dVector>> Distance_shape_surface(
//    const std::vector<std::vector<ON_3dPoint>>& A,
//    const std::vector<std::vector<ON_3dPoint>>& B);

std::vector<TRow> buildTMesh_pro(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt,
    const std::vector<ON_NurbsCurve>& allguidecurves,
    std::vector<std::vector<double>>& Adaptive_knots,
    int degree);