#include "Penaltyfunc.h"
//--------------------------------------
// 用 OpenNURBS 函数计算基函数及导数
//--------------------------------------
BasisResult computeBasisAndDerivatives_ON(
    int degree,
    const std::vector<double>& knots,
    double t)
{
    BasisResult res;
    int order = degree + 1;
    int n = (int)knots.size() + 2 - order;//控制点数量
    if (n <= 0) return res;

    res.N.assign(n, 0.0);
    res.dN.assign(n, 0.0);
    res.ddN.assign(n, 0.0);

    // 找到参数所在的span
    int span = -1;
    for (int i = degree - 1; i < n; ++i) {
        if (t >= knots[i] && t < knots[i + 1]) {
            span = i;
            break;
        }
    }
    if (span == -1) {
        if (fabs(t - knots[n - 1]) < 1e-12)
            span = (int)knots.size() - degree - 1;
        else
            return res;
    }
    //std::cout << "span=" << span << std::endl;

    const double* spanKnot = &knots[span - degree + 1];


    std::vector<double> N(order * order, 0.0);

    // Step 1: 计算基函数
    ON_EvaluateNurbsBasis(order, spanKnot, t, N.data());

    // Step 2: 计算导数 (1阶和2阶)
    ON_EvaluateNurbsBasisDerivatives(order, spanKnot, 2, N.data());

    // Step 3: 写入结果
    for (int j = 0; j < order; ++j) {
        int idx = span - degree + 1 + j;
        if (idx >= 0 && idx < n) {
            res.N[idx] = N[0 * order + j];
            res.dN[idx] = N[1 * order + j];
            res.ddN[idx] = N[2 * order + j];
        }
    }

    return res;
}
//std::cout << "=== t = " << t << " ===\n";
//for (size_t i = 0; i < result.N.size(); ++i) {
//    std::cout << "N[" << i << "]   = " << result.N[i]
//        << "  dN[" << i << "]  = " << result.dN[i]
//        << "  ddN[" << i << "] = " << result.ddN[i]
//        << std::endl;
//}
// 
// 






//--------------------------------------
// 主函数：计算 (S, Su, Sv, Suu, Suv, Svv)
//--------------------------------------
SurfaceDerivatives computeSurfaceDerivatives_Tspline(
    double u, double v,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v)
{
    SurfaceDerivatives res;

    // 分子和分母累加器
    ON_3dPoint  R(0, 0, 0);
    ON_3dVector Ru(0, 0, 0), Rv(0, 0, 0);
    ON_3dVector Ruu(0, 0, 0), Rvv(0, 0, 0), Ruv(0, 0, 0);

    double W = 0, Wu = 0, Wv = 0, Wuu = 0, Wvv = 0, Wuv = 0;

    // V方向基函数与导数
    BasisResult M_vals = computeBasisAndDerivatives_ON(degree_v, v_knots, v);
    //    std::cout << "=== v = " << v << " ===\n";
    //for (size_t i = 0; i < M_vals.N.size(); ++i) {
    //    std::cout << "N[" << i << "]   = " << M_vals.N[i]
    //        << "  dN[" << i << "]  = " << M_vals.dN[i]
    //        << "  ddN[" << i << "] = " << M_vals.ddN[i]
    //        << std::endl;
    //}
        // 遍历TMesh的每一行
    for (size_t i = 0; i < tmesh.size(); ++i) {
        const TRow& row = tmesh[i];

        double M = M_vals.N[i];
        double Md = M_vals.dN[i];
        double Mdd = M_vals.ddN[i];

        // 对应的U方向节点向量
        int orig_idx = findOriginalCurveForInterpolatedRow(tmesh, i);
        const ON_NurbsCurve& orig_curve = curves[orig_idx];

        std::vector<double> u_knot;
        for (int k = 0; k < orig_curve.KnotCount(); ++k)
            u_knot.push_back(orig_curve.Knot(k));

        // U方向基函数与导数
        BasisResult N_vals = computeBasisAndDerivatives_ON(degree_u, u_knot, u);

        // 遍历行内控制点
        for (size_t j = 0; j < row.control_points.size(); ++j) {
            const ON_3dPoint& P = row.control_points[j];
            double w = 1.0; // 默认权重

            double N = N_vals.N[j];
            double Nd = N_vals.dN[j];
            double Ndd = N_vals.ddN[j];

            // 组合基函数
            double B = N * M;
            double Bu = Nd * M;
            double Bv = N * Md;
            double Buu = Ndd * M;
            double Bvv = N * Mdd;
            double Buv = Nd * Md;

            // 分子累加
            R += P * (B * w);
            Ru += P * (Bu * w);
            Rv += P * (Bv * w);
            Ruu += P * (Buu * w);
            Rvv += P * (Bvv * w);
            Ruv += P * (Buv * w);

            // 分母累加
            W += B * w;
            Wu += Bu * w;
            Wv += Bv * w;
            Wuu += Buu * w;
            Wvv += Bvv * w;
            Wuv += Buv * w;
        }
    }

    // ---- 计算最终导数 ----
    res.S = R / W;
    res.Su = (Ru * W - R * Wu) / (W * W);
    res.Sv = (Rv * W - R * Wv) / (W * W);

    res.Suu = (Ruu * W - 2 * Ru * Wu - R * Wuu) / (W * W) + (2 * R * (Wu * Wu)) / (W * W * W);
    res.Svv = (Rvv * W - 2 * Rv * Wv - R * Wvv) / (W * W) + (2 * R * (Wv * Wv)) / (W * W * W);
    res.Suv = (Ruv * W - Ru * Wv - Rv * Wu - R * Wuv) / (W * W) + (2 * R * Wu * Wv) / (W * W * W);

    return res;
}



//// 5️⃣ 调用求导函数
//SurfaceDerivatives d = computeSurfaceDerivatives_Tspline(
//    u, v, tmesh, allcurves, knotsv, 3, 3
//);
//
//// 6️⃣ 输出结果
//std::cout << "S(u,v) = (" << d.S.x << ", " << d.S.y << ", " << d.S.z << ")\n";
//std::cout << "Su     = (" << d.Su.x << ", " << d.Su.y << ", " << d.Su.z << ")\n";
//std::cout << "Sv     = (" << d.Sv.x << ", " << d.Sv.y << ", " << d.Sv.z << ")\n";
//std::cout << "Suu    = (" << d.Suu.x << ", " << d.Suu.y << ", " << d.Suu.z << ")\n";
//std::cout << "Svv    = (" << d.Svv.x << ", " << d.Svv.y << ", " << d.Svv.z << ")\n";
//std::cout << "Suv    = (" << d.Suv.x << ", " << d.Suv.y << ", " << d.Suv.z << ")\n";

//
//
////计算曲面离心率泛函
//double computeEccentricityFunctional_Gauss5(
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& curves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v
//) {
//    // 五点高斯积分节点和权重 [-1,1]
//    const double xi[5] = { 0.0,
//        -0.5384693101056831,
//         0.5384693101056831,
//        -0.9061798459386640,
//         0.9061798459386640 };
//    const double w[5] = { 0.5688888888888889,
//        0.4786286704993665,
//        0.4786286704993665,
//        0.2369268850561891,
//        0.2369268850561891 };
//
//    double Qe = 0.0;
//
//    for (int i = 0; i < 5; ++i) {
//        double u = 0.5 * (xi[i] + 1.0); // [-1,1] -> [0,1]
//        double wu = 0.5 * w[i];
//
//        for (int j = 0; j < 5; ++j) {
//            double v = 0.5 * (xi[j] + 1.0);
//            double wv = 0.5 * w[j];
//
//            // 计算曲面一阶、二阶导
//            SurfaceDerivatives d = computeSurfaceDerivatives_Tspline(
//                u, v, tmesh, curves, v_knots, degree_u, degree_v
//            );
//
//            // 投影计算
//            double termU = (d.Su * d.Suu) / d.Su.LengthSquared(); // (S_u · S_uu) / |S_u|^2
//            double termV = (d.Sv * d.Svv) / d.Sv.LengthSquared(); // (S_v · S_vv) / |S_v|^2
//
//            Qe += (termU * termU + termV * termV) * wu * wv;
//        }
//    }
//
//    return Qe;
//}





// 传入 computeSurfaceDerivatives_Tspline 的返回值 res
CurvatureResult computeCurvaturesFromDerivatives(const SurfaceDerivatives& res) {
    CurvatureResult out;
    const double EPS = 1e-12;

    // 切向导数
    ON_3dVector Su = res.Su;
    ON_3dVector Sv = res.Sv;

    // 计算单位法向量（Su x Sv）
    ON_3dVector n = ON_CrossProduct(Su, Sv);
    double nlen = n.Length();
    if (nlen < EPS) {
        // 切向几何退化（可能是奇异点或参数映射不可逆）
        out.valid = false;
        return out;
    }
    n.Unitize();
    out.normal = n;

    // 第一基本形式系数
    double E = Su * Su; // ON 里 operator* 为点乘
    double F = Su * Sv;
    double G = Sv * Sv;

    double denom = (E * G - F * F);
    if (fabs(denom) < EPS) {
        // 度量退化（参数方向线性相关）
        out.valid = false;
        return out;
    }

    // 二阶导
    ON_3dVector Suu = res.Suu;
    ON_3dVector Suv = res.Suv;
    ON_3dVector Svv = res.Svv;

    // 第二基本形式系数
    double e = n * Suu;
    double f = n * Suv;
    double g = n * Svv;

    // 平均曲率 H 和高斯曲率 K
    double H = (e * G - 2.0 * f * F + g * E) / (2.0 * denom);
    out.H = H;
    out.valid = true;
    return out;
}


std::vector<std::vector<SamplePointmeancurvature>>
sampleSurfaceCurvatures(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max ,
    double v_min, double v_max )
{
    // 2D 网格 (Nu x Nv)
    std::vector<std::vector<SamplePointmeancurvature>>
        grid(Nu, std::vector<SamplePointmeancurvature>(Nv));

    for (int iu = 0; iu < Nu; ++iu) {

        double u = u_min + (u_max - u_min) * (double)iu / (Nu - 1);

        for (int iv = 0; iv < Nv; ++iv) {

            double v = v_min + (v_max - v_min) * (double)iv / (Nv - 1);

            SamplePointmeancurvature& sp = grid[iu][iv];

            sp.u = u;
            sp.v = v;

            // ---- 1. 计算曲面导数 ----
            sp.der = computeSurfaceDerivatives_Tspline(
                u, v, tmesh, curves, v_knots, degree_u, degree_v);

            // ---- 2. 计算曲率 ----
            sp.curv = computeCurvaturesFromDerivatives(sp.der);

            sp.valid = sp.curv.valid;
        }
    }

    return grid;
}


void computeGlobalMeanCurvature(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max)
{
    // 调用你已有的采样函数
    auto grid = sampleSurfaceCurvatures(
        Nu, Nv,
        tmesh, curves, v_knots,
        degree_u, degree_v,
        u_min, u_max,
        v_min, v_max);

    double Hmax = -1e100;
    double Hmin = 1e100;

    double u_at_Hmax = 0.0, v_at_Hmax = 0.0;
    double u_at_Hmin = 0.0, v_at_Hmin = 0.0;

    bool hasValue = false;

    // 查找曲率的最大值与最小值
    for (int iu = 0; iu < Nu; ++iu)
    {
        for (int iv = 0; iv < Nv; ++iv)
        {
            const auto& sp = grid[iu][iv];
            if (!sp.valid) continue;

            hasValue = true;

            double H = sp.curv.H;

            if (H > Hmax) {
                Hmax = H;
                u_at_Hmax = sp.u;
                v_at_Hmax = sp.v;
            }

            if (H < Hmin) {
                Hmin = H;
                u_at_Hmin = sp.u;
                v_at_Hmin = sp.v;
            }
        }
    }

    if (!hasValue) {
        std::cout << "曲面无可用曲率数据！" << std::endl;
        return;
    }

    // ----- 输出最大/最小平均曲率 -----
    std::cout << "===== Global Mean Curvature =====\n";
    std::cout << "Max H = " << Hmax
        << "  at (u,v) = (" << u_at_Hmax << ", " << v_at_Hmax << ")\n";

    std::cout << "Min H = " << Hmin
        << "  at (u,v) = (" << u_at_Hmin << ", " << v_at_Hmin << ")\n";
    // ----- 输出最大最小曲率差值 -----
    std::cout << "Difference Hmax - Hmin = " << (Hmax - Hmin) << "\n";
}
