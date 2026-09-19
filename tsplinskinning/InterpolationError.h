#pragma once

#include <vector>
#include "opennurbs.h"

// 前向声明
struct TRow;

///**
// * @brief 计算原始输入曲线到 T-spline 蒙皮曲面的最大几何距离
// *
// * 数学定义：
// *
// * E_interp = max_i max_t min_(u,v) || C_i(t) - S(u,v) ||
// *
// * 其中：
// * C_i(t) : 第 i 条原始输入曲线
// * S(u,v) : T-spline 蒙皮曲面
// *
// * 数值实现：
// * 1. 对每条原始输入曲线进行离散采样
// * 2. 对 T-spline 曲面进行参数域离散采样
// * 3. 对每一个曲线采样点，寻找最近的曲面采样点
// * 4. 对所有曲线采样点取最大距离
// *
// * @param originalCurves     最原始的输入曲线
// * @param tmesh              已经构造完成的 T-mesh
// * @param v_knots            V 方向 knot vector
// * @param degree_u           U 方向 B-spline degree
// * @param degree_v           V 方向 B-spline degree
// * @param curveSamples       每条原始曲线的采样点数量
// *
// * @return 最大几何距离，即 Interpolation Error
// **/
double ComputeInterpolationError(
    const std::vector<ON_NurbsCurve>& originalCurves,
    const std::vector<ON_NurbsCurve>& surfaceCurves,
    const std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v,
    int curveSamples);


// 计算两条 NURBS 曲线之间的离散 Hausdorff 距离
//
// 对两条曲线分别进行采样。
// 对于曲线1上的每个采样点，寻找其到曲线2采样点集合的最小距离；
// 然后取这些最小距离中的最大值。
// 同样计算曲线2到曲线1的距离。
// 最终返回两个方向中的较大值。
//
// sampleCount：每条曲线的采样点数量
double ComputeCurveDistance(
    const ON_NurbsCurve& curve1,
    const ON_NurbsCurve& curve2,
    int sampleCount);

// 计算两组对应 NURBS 截线之间的最大几何距离
//
// originalCurves：原始输入截线
// approximateCurves：近似后的截线
// sampleCount：每条曲线的采样点数量
//
// 返回值：
// 所有对应截线距离中的最大值
double ComputeCurveGroupDistance(
    const std::vector<ON_NurbsCurve>& originalCurves,
    const std::vector<ON_NurbsCurve>& approximateCurves,
    int sampleCount);