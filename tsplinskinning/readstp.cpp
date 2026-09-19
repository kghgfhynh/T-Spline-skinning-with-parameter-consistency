#include "readstp.h"
static std::vector<double> expandKnots(
    const TColStd_Array1OfReal& knots,
    const TColStd_Array1OfInteger& mults)
{
    std::vector<double> result;

    for (int i = knots.Lower(); i <= knots.Upper(); ++i)
    {
        for (int k = 0; k < mults(i); ++k)
            result.push_back(knots(i));
    }
    return result;
}

static std::vector<double> computeGreville(
    const std::vector<double>& knot,
    int degree,
    int nCtrl)
{
    std::vector<double> g(nCtrl);

    for (int i = 0; i < nCtrl; ++i)
    {
        double sum = 0.0;
        for (int k = 1; k <= degree; ++k)
            sum += knot[i + k];

        g[i] = sum / degree;
    }
    return g;
}



void ExtractIsoVCurvesFromSurface(
    const Handle(Geom_BSplineSurface)& s,
    std::vector<ON_NurbsCurve>& curves,
    std::vector<double>& v_knots,
    std::vector<double>& v_params)
{
    int uCount = s->NbUPoles();//控制点网格尺寸
    int vCount = s->NbVPoles();

    int degree_u = s->UDegree();
    int degree_v = s->VDegree();

    // ---------- 展开 knot ----------
    std::vector<double> U = expandKnots(
        s->UKnots(), s->UMultiplicities());

    std::vector<double> V = expandKnots(
        s->VKnots(), s->VMultiplicities());

    // ---------- 归一化 U ----------
    double u_min = U.front();
    double u_max = U.back();
    double u_scale = std::max(u_max - u_min, 1e-12);

    for (auto& u : U)
        u = (u - u_min) / u_scale;

    // ---------- 归一化 V ----------
    double v_min = V.front();
    double v_max = V.back();
    double v_scale = std::max(v_max - v_min, 1e-12);

    for (auto& v : V)
        v = (v - v_min) / v_scale;

    // ---------- 输出归一化 V knot ----------
    v_knots = V;

    // ---------- Greville（基于归一化 V） ----------
    v_params = computeGreville(V, degree_v, vCount);

    // ---------- 曲线 ----------
    curves.clear();
    curves.reserve(vCount);

    for (int j = 0; j < vCount; ++j)
    {
        std::vector<ON_3dPoint> ctrl;
        ctrl.reserve(uCount);

        for (int i = 0; i < uCount; ++i)
        {
            gp_Pnt p = s->Pole(i + 1, j + 1);
            ctrl.emplace_back(p.X(), p.Y(), p.Z());
        }

        std::vector<double> weights(uCount, 1.0);

        int dim = 3;
        int order = degree_u + 1;

        ON_NurbsCurve curve(dim, true, order, uCount);

        // ---------- CV + weight ----------
        for (int i = 0; i < uCount; ++i)
        {
            curve.SetCV(i, ctrl[i]);
            curve.SetWeight(i, weights[i]);
        }

        // ---------- knot（已归一化）----------
        int knotCount = uCount + degree_u - 1;

        for (int k = 0; k < knotCount; ++k)
        {
            curve.SetKnot(k, U[k + 1]);  // ✔ 仍然 drop 两端
        }

        curves.push_back(curve);
    }
}

std::vector<TRow> buildTMeshFromCurves(
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_params)
{
    std::vector<TRow> tmesh;

    int vCount = curves.size();
    if (vCount == 0) return tmesh;

    int uCount = curves[0].CVCount();

    tmesh.reserve(vCount);

    for (int j = 0; j < vCount; ++j)
    {
        const ON_NurbsCurve& c = curves[j];

        TRow row;

        // ---------- v 参数（直接使用已有 Greville） ----------
        row.v = v_params[j];
        row.control_points.resize(uCount);
        row.u_positions = grevilleAbscissae(curves[j]);
        // ---------- 从 curve 提取 ----------
        for (int i = 0; i < uCount; ++i)
        {
            // 控制点（齐次 → 欧氏）
            ON_4dPoint cv;
            c.GetCV(i, cv);

            row.control_points[i] =
                ON_3dPoint(cv.x / cv.w,
                    cv.y / cv.w,
                    cv.z / cv.w);
        }

        tmesh.push_back(row);
    }

    return tmesh;
}




//--------------------------------------
// 主函数：计算 (S, Su, Sv, Suu, Suv, Svv)
//--------------------------------------
SurfaceDerivatives computeSurfaceDerivatives_Tspline_stp(
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
        
        const ON_NurbsCurve& orig_curve = curves[i];

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




std::vector<std::vector<SamplePointmeancurvature>>
sampleSurfaceCurvatures_stp(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max)
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
            sp.der = computeSurfaceDerivatives_Tspline_stp(
                u, v, tmesh, curves, v_knots, degree_u, degree_v);

            // ---- 2. 计算曲率 ----
            sp.curv = computeCurvaturesFromDerivatives(sp.der);

            sp.valid = sp.curv.valid;
        }
    }

    return grid;
}


void computeGlobalMeanCurvature_stp(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max)
{
    // 调用你已有的采样函数
    auto grid = sampleSurfaceCurvatures_stp(
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
