#pragma once
#include "Penaltyfunc.h"
#include <Eigen/Sparse>
#include <fstream>

//--------------------------------------
// 只计算u方向的薄板能量
//--------------------------------------
double computeUBendingEnergy_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
);
//--------------------------------------
// 计算u,v方向的薄板能量
//--------------------------------------
double computeUniformityFunctional_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
);
//--------------------------------------
// 计算 dSuu / dWji单个控制点的偏导数
//--------------------------------------
double compute_dSuu_dWij(
    size_t row_i,        // i: 插入行 index
    size_t col_j,        // j: 控制点 index
    double u, double v,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v);
//--------------------------------------
// 计算 dE / dWji单个控制点的偏导数
//--------------------------------------
double compute_dEu_dWij_Gauss5(
    size_t row_i,
    size_t col_j,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v);

bool isWijUp(size_t row_i, const std::vector<TRow>& tmesh);

std::vector<std::vector<double>>compute_dEu_dWij_up_matrix(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v);

void update_tmesh_Wij_up(
    std::vector<TRow>& tmesh,
    const std::vector<std::vector<double>>& grad,
    double alpha
);

//这里的tmesh是更新过wup的tmesh,不满足约束条件
void update_tmesh_Wij_down_with_interpolation(
    std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_v
);
//梯度下降法
void gradientDescentSurfaceOptimization(
    std::vector<TRow>& tmesh_before,
    std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v,
    double alpha,
    int max_iters,
    double tol);





//计算刚度矩阵K，EKu​=pTKu​p
Eigen::SparseMatrix<double>computeKu_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
);
//导出刚度矩阵到matlab中
void exportKu(
    const Eigen::SparseMatrix<double>& Ku,
    const std::vector<TRow>& tmesh,
    const std::string& filename);
//导出初始的控制点
void exportCtrlPoints(
    const std::vector<TRow>& tmesh,
    const std::string& filename);
//导出每行控制点的个数
void printAndExportRowSizes(const std::vector<TRow>& tmesh, const std::string& filename);
//导出约束条件需要的系数
void exportABCforDown(
    const std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_v,
    const std::string& filename
);
void importCtrlPointsToTmesh(std::vector<TRow>& tmesh, const std::string& filename);
//验证结果是否满足插值条件
void verifyControlPointsInterpolation(
    const std::vector<TRow>& tmesh_before,
    const std::vector<TRow>& tmesh_after,
    const std::vector<double>& v_knots,
    int degree_v,
    double tolerance
);

void verifyControlPointsInterpolation_aftergrad(
    const std::vector<TRow>& tmesh_before,
    const std::vector<TRow>& tmesh_after,
    const std::vector<double>& v_knots,
    int degree_v,
    double tolerance
);