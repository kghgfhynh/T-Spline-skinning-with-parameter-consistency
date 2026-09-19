#pragma once
#define _USE_MATH_DEFINES
#include <opennurbs.h>
#include "paramatch.h"
#include <vector>
#include <tuple>
#include <string>
#include <iostream>

// T样条Mesh行结构体
struct TRow {
    double v;                              // V方向参数位置
    std::vector<double> u_positions;      // U方向 Greville abscissae
    std::vector<ON_3dPoint> control_points;// 控制点
};

// 计算Greville参数点
std::vector<double> grevilleAbscissae(const ON_NurbsCurve& curve);

void initInsertedRow(TRow& row, const ON_NurbsCurve& c1, const ON_NurbsCurve& c2, bool isUp);

// 构建T样条网格
std::vector<TRow> buildTMesh(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt);

// 打印T样条网格
void printTMesh(const std::vector<TRow>& tmesh);

// 从Greville点计算节点向量
std::vector<double> knotsFromGreville(const std::vector<double>& greville, int degree);

// 收集T样条网格每行的v参数
std::vector<double> collectRowVs(const std::vector<TRow>& tmesh);

// 计算基函数
std::vector<double> computeBasisFunctionsAt(int degree, const std::vector<double>& knots, double t);

// 计算V方向权重三元组
std::tuple<double, double, double> computeVWeightsForRow(
    const std::vector<double>& v_knots, int degree_v, double v_param, int i);

// 测试基函数矩阵打印
void testBasisMatrix(const std::vector<double>& knots, int degree,
    const std::vector<double>& params);

// 更新控制点应用插值条件
void updateControlPointsWithInterpolation(std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots, int degree_v);

// 查找插值行对应的原始曲线索引
int findOriginalCurveForInterpolatedRow(const std::vector<TRow>& tmesh, int row_index);

// 计算曲面点
ON_3dPoint computeSurfacePoint(
    double u, double v,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v);

// 保存曲面到OBJ文件
void saveSurfaceToObjFile(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max,
    int u_samples, int v_samples,
    const std::string& filename);
