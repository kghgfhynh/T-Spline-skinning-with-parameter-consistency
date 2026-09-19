#include "InterpolationError.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

// 这里需要能够使用：
// computeSurfacePoint()
// TRow
//
// 如果你的 computeSurfacePoint() 定义在 Ohmethod.h 中，
// 就包含对应的头文件。
#include "Ohmethod.h"


double ComputeInterpolationError(
    const std::vector<ON_NurbsCurve>& originalCurves,
    const std::vector<ON_NurbsCurve>& surfaceCurves,
    const std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v,
    int curveSamples)
{
    // ============================================================
    // 1. 基本检查
    // ============================================================

    if (originalCurves.empty())
    {
        std::cerr << "Error: originalCurves is empty."
            << std::endl;
        return -1.0;
    }

    if (tmesh.empty())
    {
        std::cerr << "Error: tmesh is empty."
            << std::endl;
        return -1.0;
    }

    if (v_knots.empty())
    {
        std::cerr << "Error: v_knots is empty."
            << std::endl;
        return -1.0;
    }

    if (curveSamples < 2)
    {
        std::cerr << "Error: curveSamples must be >= 2."
            << std::endl;
        return -1.0;
    }


    // ============================================================
    // 2. 检查原始截面数量与 T-mesh
    // ============================================================

    if (originalCurves.size() > tmesh.size())
    {
        std::cerr
            << "Error: not enough T-mesh rows for "
            << "the original cross-sections."
            << std::endl;

        return -1.0;
    }


    // ============================================================
    // 3. U 参数域
    // ============================================================

    const ON_Interval u_domain =
        originalCurves[0].Domain();

    const double u_min =
        u_domain.Min();

    const double u_max =
        u_domain.Max();


    // ============================================================
    // 4. 计算最大 interpolation error
    // ============================================================

    double maxError = 0.0;


    // ------------------------------------------------------------
    // 对每一条原始截面
    // ------------------------------------------------------------

    for (size_t curveIndex = 0;
        curveIndex < originalCurves.size();
        ++curveIndex)
    {
        const ON_NurbsCurve& curve =
            originalCurves[curveIndex];


        // --------------------------------------------------------
        // 当前截面的 v 参数
        // --------------------------------------------------------

        double v =
            tmesh[curveIndex*3].v;


        // --------------------------------------------------------
        // 当前曲线自己的参数域
        // --------------------------------------------------------

        ON_Interval domain =
            curve.Domain();

        double t_min =
            domain.Min();

        double t_max =
            domain.Max();


        // --------------------------------------------------------
        // 在 U 方向进行参数对应采样
        // --------------------------------------------------------

        for (int j = 0;
            j < curveSamples;
            ++j)
        {
            double u_normalized =
                static_cast<double>(j)
                /
                static_cast<double>(
                    curveSamples - 1);


            // ----------------------------------------------------
            // 当前原始曲线参数
            // ----------------------------------------------------

            double t =
                t_min
                +
                u_normalized
                *
                (t_max - t_min);


            // ----------------------------------------------------
            // 原始截面上的点
            // ----------------------------------------------------

            ON_3dPoint curvePoint =
                curve.PointAt(t);


            // ----------------------------------------------------
            // 曲面上的对应点
            //
            // u 使用相同的归一化参数
            // v 使用当前截面的实际 v_i
            // ----------------------------------------------------

            double u =
                u_min
                +
                u_normalized
                *
                (u_max - u_min);


            ON_3dPoint surfacePoint =
                computeSurfacePoint(
                    u,
                    v,
                    tmesh,
                    surfaceCurves,
                    v_knots,
                    degree_u,
                    degree_v
                );


            // ----------------------------------------------------
            // 参数对应点之间的距离
            // ----------------------------------------------------

            double distance =
                curvePoint.DistanceTo(
                    surfacePoint
                );


            // ----------------------------------------------------
            // 更新最大误差
            // ----------------------------------------------------

            if (distance > maxError)
            {
                maxError = distance;
            }
        }
    }


    // ============================================================
    // 5. 返回最大 interpolation error
    // ============================================================

    return maxError;
}
double ComputeCurveDistance(
    const ON_NurbsCurve& curve1,
    const ON_NurbsCurve& curve2,
    int sampleCount)
{
    if (sampleCount < 2)
    {
        return 0.0;
    }

    // ============================================================
    // 1. 采样第一条曲线
    // ============================================================

    std::vector<ON_3dPoint> points1;
    points1.reserve(sampleCount);

    double start1 = curve1.Domain().Min();
    double end1 = curve1.Domain().Max();

    for (int i = 0; i < sampleCount; ++i)
    {
        double s = static_cast<double>(i) /
            static_cast<double>(sampleCount - 1);

        double u = start1 + s * (end1 - start1);

        points1.push_back(curve1.PointAt(u));
    }


    // ============================================================
    // 2. 采样第二条曲线
    // ============================================================

    std::vector<ON_3dPoint> points2;
    points2.reserve(sampleCount);

    double start2 = curve2.Domain().Min();
    double end2 = curve2.Domain().Max();

    for (int i = 0; i < sampleCount; ++i)
    {
        double s = static_cast<double>(i) /
            static_cast<double>(sampleCount - 1);

        double u = start2 + s * (end2 - start2);

        points2.push_back(curve2.PointAt(u));
    }


    // ============================================================
    // 3. 计算 curve1 -> curve2
    //
    // 对 curve1 上每一个点：
    //     找到它到 curve2 的最近点
    //
    // 然后取所有这些最近距离中的最大值
    // ============================================================

    double maxDistance12 = 0.0;

    for (const ON_3dPoint& p1 : points1)
    {
        double minDistance = 1.0e300;

        for (const ON_3dPoint& p2 : points2)
        {
            double dx = p1.x - p2.x;
            double dy = p1.y - p2.y;
            double dz = p1.z - p2.z;

            double distance =
                std::sqrt(
                    dx * dx +
                    dy * dy +
                    dz * dz
                );

            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }

        if (minDistance > maxDistance12)
        {
            maxDistance12 = minDistance;
        }
    }


    // ============================================================
    // 4. 计算 curve2 -> curve1
    // ============================================================

    double maxDistance21 = 0.0;

    for (const ON_3dPoint& p2 : points2)
    {
        double minDistance = 1.0e300;

        for (const ON_3dPoint& p1 : points1)
        {
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;
            double dz = p2.z - p1.z;

            double distance =
                std::sqrt(
                    dx * dx +
                    dy * dy +
                    dz * dz
                );

            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }

        if (minDistance > maxDistance21)
        {
            maxDistance21 = minDistance;
        }
    }


    // ============================================================
    // 5. 双向 Hausdorff 距离
    // ============================================================

    if (maxDistance12 > maxDistance21)
    {
        return maxDistance12;
    }
    else
    {
        return maxDistance21;
    }
}

double ComputeCurveGroupDistance(
    const std::vector<ON_NurbsCurve>& originalCurves,
    const std::vector<ON_NurbsCurve>& approximateCurves,
    int sampleCount)
{
    // 检查两组曲线数量是否一致
    if (originalCurves.size() != approximateCurves.size())
    {
        return -1.0;
    }

    if (originalCurves.empty())
    {
        return 0.0;
    }

    double maxGroupDistance = 0.0;

    for (size_t i = 0; i < originalCurves.size(); ++i)
    {
        double distance = ComputeCurveDistance(
            originalCurves[i],
            approximateCurves[i],
            sampleCount
        );

        if (distance > maxGroupDistance)
        {
            maxGroupDistance = distance;
        }
    }

    return maxGroupDistance;
}