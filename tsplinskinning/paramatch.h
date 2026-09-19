#pragma once
#define _USE_MATH_DEFINES
#include <opennurbs.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <Eigen/Dense>
std::vector<double> generateUniformKnots(int cv_count, int degree);
// 弦长参数化，返回参数化数组u
std::vector<double> chordLengthParam(std::vector<ON_3dPoint>& points);

// 创建Nurbs曲线
ON_NurbsCurve CreateNURBSCurve(
    int dimension,
    bool isRational,
    int order,
    const std::vector<ON_3dPoint>& controlPoints,
    const std::vector<double>& weights,
    const std::vector<double>& knots
);

// 输出单条NURBS曲线的阶数、控制点数量、控制点、节点向量及权重
void PrintNURBSCurveInfo(const ON_NurbsCurve& curve);

// 采样一条NURBS曲线的点集，等参数间隔采样,这个采样只是为了看出连线的扭曲
std::vector<ON_3dPoint> sampleCurve(const ON_NurbsCurve& curve, int num_samples);
std::vector<ON_3dPoint> sampleCurveByArcLength(
    const ON_NurbsCurve& curve,
    int num_samples);
// 写多条曲线顶点和折线，支持多条曲线之间横向连线
// crossLinesBetweenCurves为true时，写相邻曲线间参数对应点的横线连接线段
void writeMultiCurvesWithCrossLinesToOBJ(
    const std::vector<std::vector<ON_3dPoint>>& curves_points,
    const std::string& filename,
    bool crossLinesBetweenCurves = true);

//将曲线通过区间裁剪
std::vector<ON_NurbsCurve> SplitNurbsBySpans(const ON_NurbsCurve& curve);

//打印曲线的向量
void PrintNurbsSegments(const std::vector<ON_NurbsCurve>& segs);

//估算二阶导数上界
double EstimateSecondDerivativeBound(const ON_NurbsCurve& c, int sampleN);

// 在一段 NURBS 曲线上采样点
std::vector<ON_3dPoint> SampleNurbsSegment(const ON_NurbsCurve& c, double epsilon);
//auto segmentsA = SplitNurbsBySpans(prepared);
//PrintNurbsSegments(segmentsA);
//double epsilon = 1e-3; // 误差容忍度
//for (size_t i = 0; i < segmentsA.size(); ++i)
//{
//    auto pts = SampleNurbsSegment(segmentsA[i], epsilon);
//    /*    std::cout << "Segment " << i << " sampled " << pts.size() << " points\n";
//        for (size_t j = 0; j < pts.size(); ++j)
//        {
//            std::cout << "   P" << j << " = ("
//                << pts[j].x << ", " << pts[j].y << ", " << pts[j].z << ")\n";
//        }*/
//}

//将曲线采点“Surface skinning revisited”
std::vector<ON_3dPoint>SampleCurveBySpans(const ON_NurbsCurve& curve, double epsilon);
//std::cout << "Total sampled points: " << sampledPts.size() << std::endl;
//    for (size_t i = 0; i < sampledPts.size(); ++i)
//    {
//        std::cout << "P" << i << " = ("
//            << sampledPts[i].x << ", "
//            << sampledPts[i].y << ", "
//            << sampledPts[i].z << ")\n";
//    }


//采点可视化
//bool  SavePointsToOBJ(const std::string& filename,
//    const std::vector<std::vector<ON_3dPoint>>& curves);
//if (SavePointsToOBJ("resampled_curves.obj", AllReSampledCurves))
//{
//    std::cout << "Saved curves to resampled_curves.obj" << std::endl;
//}

//保存参数文件
void saveParametersToFile(const std::vector<double>& params, const std::string& filename);
//保存采样点到文件中用于matlab
void savePointToFile(const std::vector<ON_3dPoint>& points, const std::string& filename);
// 平均节点法生成节点向量
std::vector<double> averageKnotsvector(const std::vector<double>& parameters, int degree, int numCtrlPts);

// 生成均匀开放B样条节点向量
std::vector<double> uniformKnotVector(int numCtrlPts, int degree);

//求解控制点
void SolveControlPoints_OpenNurbs(
    int degree,                                       // 曲线次数-1，即degree=p
    const std::vector<double>& knotVector,           // 该NURBS的节点向量（非重复端点）
    const std::vector<double>& params,                // 参数点(u_0,u_1,...),每个对应一个空间点
    const std::vector<ON_3dPoint>& points,            // 空间点序列，对应参数点的曲线点坐标Y
    std::vector<ON_3dPoint>& controlPoints            // 输出的控制点序列Q，反求得
);


std::vector<double> readFileSkipFirstLast(const std::string& filename);