#include "Optimization_fun.h"
double computeUBendingEnergy_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
) {
    // 五点高斯积分点和权重
    const double xi[5] = { 0.0,
        -0.5384693101056831,
         0.5384693101056831,
        -0.9061798459386640,
         0.9061798459386640 };
    const double w[5] = { 0.5688888888888889,
        0.4786286704993665,
        0.4786286704993665,
        0.2369268850561891,
        0.2369268850561891 };

    double Q = 0.0;

    for (int i = 0; i < 5; ++i) {
        double u = 0.5 * (xi[i] + 1.0);
        double wu = 0.5 * w[i];  // 区间映射

        for (int j = 0; j < 5; ++j) {
            double v = 0.5 * (xi[j] + 1.0);
            double wv = 0.5 * w[j];

            SurfaceDerivatives d = computeSurfaceDerivatives_Tspline(
                u, v, tmesh, curves, v_knots, degree_u, degree_v
            );

            double integrand = d.Suu.LengthSquared();
            Q += integrand * wu * wv;  // 权重累加
        }
    }

    return Q;
}
//计算曲面均匀性泛函
double computeUniformityFunctional_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
) {
    // 五点高斯积分点和权重
    const double xi[5] = { 0.0,
        -0.5384693101056831,
         0.5384693101056831,
        -0.9061798459386640,
         0.9061798459386640 };
    const double w[5] = { 0.5688888888888889,
        0.4786286704993665,
        0.4786286704993665,
        0.2369268850561891,
        0.2369268850561891 };

    double Q = 0.0;

    for (int i = 0; i < 5; ++i) {
        double u = 0.5 * (xi[i] + 1.0);
        double wu = 0.5 * w[i];  // 区间映射

        for (int j = 0; j < 5; ++j) {
            double v = 0.5 * (xi[j] + 1.0);
            double wv = 0.5 * w[j];

            SurfaceDerivatives d = computeSurfaceDerivatives_Tspline(
                u, v, tmesh, curves, v_knots, degree_u, degree_v
            );

            double integrand = d.Suu.LengthSquared() + 2.0 * d.Suv.LengthSquared() + d.Svv.LengthSquared();
            Q += integrand * wu * wv;  // 权重累加
        }
    }

    return Q;
}

//--------------------------------------
// 计算 αSuu / αWji单个控制点的偏导数
//--------------------------------------
double compute_dSuu_dWij(
    size_t row_i,        // i: 插入行 index
    size_t col_j,        // j: 控制点 index
    double u, double v,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v)
{
    // ========= 1. 计算全局 W, Wu, Wuu =========
    double W = 0.0, Wu = 0.0, Wuu = 0.0;

    for (size_t i = 0; i < tmesh.size(); ++i) {
        const TRow& row = tmesh[i];

        BasisResult Mv = computeBasisAndDerivatives_ON(degree_v, v_knots, v);
        double M = Mv.N[i];

        int orig_idx = findOriginalCurveForInterpolatedRow(tmesh, i);
        const ON_NurbsCurve& curve = curves[orig_idx];

        std::vector<double> u_knot(curve.KnotCount());
        for (int k = 0; k < curve.KnotCount(); ++k)
            u_knot[k] = curve.Knot(k);

        BasisResult Nu = computeBasisAndDerivatives_ON(degree_u, u_knot, u);

        for (size_t j = 0; j < row.control_points.size(); ++j) {
            double B = Nu.N[j] * M;
            double Bu = Nu.dN[j] * M;
            double Buu = Nu.ddN[j] * M;

            W += B;
            Wu += Bu;
            Wuu += Buu;
        }
    }

    // ========= 2. 计算当前控制点的 B, Bu, Buu =========
    BasisResult Mv = computeBasisAndDerivatives_ON(degree_v, v_knots, v);
    double M = Mv.N[row_i];

    int orig_idx = findOriginalCurveForInterpolatedRow(tmesh, row_i);
    const ON_NurbsCurve& curve = curves[orig_idx];

    std::vector<double> u_knot(curve.KnotCount());
    for (int k = 0; k < curve.KnotCount(); ++k)
        u_knot[k] = curve.Knot(k);

    BasisResult Nu = computeBasisAndDerivatives_ON(degree_u, u_knot, u);

    double B = Nu.N[col_j] * M;
    double Bu = Nu.dN[col_j] * M;
    double Buu = Nu.ddN[col_j] * M;

    // ========= 3. 套用解析公式 =========
    double dSuu =
        (Buu * W - 2.0 * Bu * Wu - B * Wuu) / (W * W)
        + (2.0 * B * Wu * Wu) / (W * W * W);

    return dSuu;
}
double compute_dEu_dWij_Gauss5(
    size_t row_i,
    size_t col_j,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v)
{
    // 五点 Gauss
    const double xi[5] = {
        0.0,
       -0.5384693101056831,
        0.5384693101056831,
       -0.9061798459386640,
        0.9061798459386640
    };
    const double w[5] = {
        0.5688888888888889,
        0.4786286704993665,
        0.4786286704993665,
        0.2369268850561891,
        0.2369268850561891
    };
    
    double grad = 0.0;

    for (int iu = 0; iu < 5; ++iu) {
        double u = 0.5 * (xi[iu] + 1.0);
        double wu = 0.5 * w[iu];

        for (int iv = 0; iv < 5; ++iv) {
            double v = 0.5 * (xi[iv] + 1.0);
            double wv = 0.5 * w[iv];

            // 1. Suu
            SurfaceDerivatives d =
                computeSurfaceDerivatives_Tspline(
                    u, v, tmesh, curves, v_knots,
                    degree_u, degree_v);

            // 2. ∂Suu/∂Wup
            double dSuu =
                compute_dSuu_dWij(
                    row_i, col_j,
                    u, v,
                    tmesh, curves, v_knots,
                    degree_u, degree_v);

            // 3. 点积（你现在是 ||Suu||²，所以等价）
            double integrand =
                d.Suu.Length() * dSuu;

            grad += 2.0 * integrand * wu * wv;
        }
    }

    return grad;
}
bool isOptimizableRow(size_t row_i, const std::vector<TRow>& tmesh)
{
    size_t N = tmesh.size();

    if (row_i >= N)
        return false;

    // 第 2 排 & 倒数第 2 排：允许优化
    if (row_i == 1 || row_i == N - 2)
        return true;

    // 中间区域的 up 行
    if (row_i >= 2 && row_i < N - 2)
        return (row_i % 3 == 2);

    return false;
}

bool isWijUp(size_t row_i, const std::vector<TRow>& tmesh)
{
    size_t N = tmesh.size();

    // ---------- 边界保护 ----------
    if (row_i >= N)
        return false;

    // ---------- 首插入行：只有 down ----------
    if (row_i == 1)
        return false;

    //// ---------- 尾插入行：只有 up ----------
    if (row_i == N - 2)
        return false;

    // ---------- 中间区域 ----------
    if (row_i >= 2 && row_i < N - 2) {
        // i % 3 == 2 → up
        return (row_i % 3 == 2);
    }

    // ---------- 其余情况（原始行） ----------
    return false;
}

std::vector<std::vector<double>>
compute_dEu_dWij_up_matrix(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v)
{
    size_t nRows = tmesh.size();
    std::vector<std::vector<double>> grad(nRows);

    for (size_t i = 0; i < nRows; ++i) {
        size_t nCtrl = tmesh[i].control_points.size();
        grad[i].assign(nCtrl, 0.0);  // 默认全 0
        // 只对 Wij_up 计算
        if (!isOptimizableRow(i, tmesh))
            continue;
        for (size_t j = 0; j < nCtrl; ++j) {

            grad[i][j] = compute_dEu_dWij_Gauss5(
                i, j,
                tmesh,
                curves,
                v_knots,
                degree_u,
                degree_v
            );
            /*std::cout << "grad[" << i << "][" << j << "] = " << grad[i][j] << std::endl;*/

        }
    }

    return grad;
}
void update_tmesh_Wij_up(
    std::vector<TRow>& tmesh,
    const std::vector<std::vector<double>>& grad,
    double alpha
) {
    size_t N = tmesh.size();

    for (size_t i = 0; i < N; ++i) {
        bool isUp = isOptimizableRow(i, tmesh);

        // 打印每一行的判定结果
        std::cout << "Row " << i
            << " | nCtrl = " << tmesh[i].control_points.size()
            << " | isWijUp = " << (isUp ? "YES" : "NO")
            << std::endl;

        if (!isUp)
            continue;


        size_t nCtrl = tmesh[i].control_points.size();
        if (grad[i].size() != nCtrl) {
            std::cerr << "Warning: grad row size mismatch at row " << i << std::endl;
            continue;
        }

        for (size_t j = 0; j < nCtrl; ++j) {
            // 梯度下降更新
            tmesh[i].control_points[j].x += alpha * grad[i][j];
            tmesh[i].control_points[j].y += alpha * grad[i][j];
            tmesh[i].control_points[j].z += alpha * grad[i][j];
        }
    }
}
////这里的tmesh是更新过wup的tmesh,不满足约束条件
void update_tmesh_Wij_down_with_interpolation(
    std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_v
) {
    size_t N = tmesh.size();

    // 遍历中间插入行，每组三行 (up, main, down)
    // 跳过首尾，i 从 3 开始，每组三行
    for (size_t i = 3; i < N - 1; i += 3) {
        TRow& row_up = tmesh[i - 1]; // 上一行 up 
        TRow& row_main = tmesh[i]; // 当前 main (Wij) 
        TRow& row_down = tmesh[i + 1]; // 下一行 down 
        double v_param = row_main.v; // 当前 main 行对应的参数 v

        // 计算插值权重 a,b,c
        double a, b, c;
        std::tie(a, b, c) = computeVWeightsForRow(v_knots, degree_v, v_param, i);
        // ======== 打印 a b c =========
        std::cout << "[V-weights] "
            << "main row i = " << i
            << " , v = " << v_param
            << " , a = " << a
            << " , b = " << b
            << " , c = " << c
            << " , a+b+c = " << (a + b + c)
            << std::endl;
        // =============================
        size_t nCtrl = row_main.control_points.size();

        for (size_t j = 0; j < nCtrl; ++j) {
            const ON_3dPoint& Wi = row_main.control_points[j];
            const ON_3dPoint& Wi_up = row_up.control_points[j];

            // 更新 down 控制点
            row_down.control_points[j] = ((1.0 - b) * Wi - a * Wi_up) / c;
        }
    }
}
void verifyControlPointsInterpolation(
    const std::vector<TRow>& tmesh_before,
    const std::vector<TRow>& tmesh_after,
    const std::vector<double>& v_knots,
    int degree_v,
    double tolerance
) {

    size_t N = tmesh_after.size();

    for (size_t i = 3; i < N - 1; i += 3) {

        const TRow& row_up_before = tmesh_before[i - 1];
        const TRow& row_main_before = tmesh_before[i];
        const TRow& row_down_before = tmesh_before[i + 1];

        const TRow& row_main_after = tmesh_after[i];

        double v_param = row_main_after.v;

        double a, b, c;
        std::tie(a, b, c) =
            computeVWeightsForRow(v_knots, degree_v, v_param, i);

        size_t nCtrl = row_main_after.control_points.size();

        for (size_t j = 0; j < nCtrl; ++j) {

            const ON_3dPoint& Wi_up_before =
                row_up_before.control_points[j];

            const ON_3dPoint& Wi_down_before =
                row_down_before.control_points[j];

            const ON_3dPoint& Wi_main_before =
                row_main_before.control_points[j];

            const ON_3dPoint& Wi_main_after =
                row_main_after.control_points[j];

            // ★ 用 before + after 混合验证
            ON_3dPoint lhs =
                a * Wi_up_before
                + b * Wi_main_after
                + c * Wi_down_before;

            double error = lhs.DistanceTo(Wi_main_before);

            if (error > tolerance) {
                std::cout
                    << "Interpolation mismatch at (row "
                    << i << ", ctrl " << j << ")\n"
                    << "  error = " << error << std::endl;
            }
        }
    }

    std::cout << "Control-point interpolation verification finished.\n";
}
void verifyControlPointsInterpolation_aftergrad(
    const std::vector<TRow>& tmesh_before,
    const std::vector<TRow>& tmesh_after,
    const std::vector<double>& v_knots,
    int degree_v,
    double tolerance
) {

    size_t N = tmesh_after.size();

    for (size_t i = 3; i < N - 1; i += 3) {

        const TRow& row_up_after = tmesh_after[i - 1];
        const TRow& row_main_before = tmesh_before[i];
        const TRow& row_down_after = tmesh_after[i + 1];

        const TRow& row_main_after = tmesh_after[i];

        double v_param = row_main_after.v;

        double a, b, c;
        std::tie(a, b, c) =
            computeVWeightsForRow(v_knots, degree_v, v_param, i);

        size_t nCtrl = row_main_after.control_points.size();

        for (size_t j = 0; j < nCtrl; ++j) {

            const ON_3dPoint& Wi_up_after =
                row_up_after.control_points[j];

            const ON_3dPoint& Wi_down_after =
                row_down_after.control_points[j];

            const ON_3dPoint& Wi_main_before =
                row_main_before.control_points[j];

            const ON_3dPoint& Wi_main_after =
                row_main_after.control_points[j];

            // ★ 用 before + after 混合验证
            ON_3dPoint lhs =
                a * Wi_up_after
                + b * Wi_main_after
                + c * Wi_down_after;

            double error = lhs.DistanceTo(Wi_main_before);

            if (error > tolerance) {
                std::cout
                    << "Interpolation mismatch at (row "
                    << i << ", ctrl " << j << ")\n"
                    << "  error = " << error << std::endl;
            }
        }
    }

    std::cout << "Control-point interpolation verification finished.\n";
}

void gradientDescentSurfaceOptimization(
    std::vector<TRow>& tmesh_before,
    std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v,
    double alpha,
    int max_iters,
    double tol
) {
    double prev_energy = computeUBendingEnergy_Gauss5(tmesh, curves, v_knots, degree_u, degree_v);
    //double prev_energy = computeUniformityFunctional_Gauss5(tmesh, curves, v_knots, degree_u, degree_v);
    for (int iter = 0; iter < max_iters; ++iter) {
        // 1. 计算梯度矩阵
        std::vector<std::vector<double>> grad = compute_dEu_dWij_up_matrix(
            tmesh, curves, v_knots, degree_u, degree_v
        );
        //saveSurfaceToObjFile(tmesh, curves, v_knots, 3, 3, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface111_begin.obj");正常贴合
        // 2. 更新 up 控制点
        update_tmesh_Wij_up(tmesh, grad, alpha);
        //printTMesh(tmesh);
        // === 3. 强制恢复 main（非常关键）===
        for (size_t i = 3; i < tmesh.size() - 1; i += 3) {
            // 从 tmesh_before 获取之前的控制点
            const TRow& row_up_before = tmesh_before[i - 1];
            const TRow& row_main_before = tmesh_before[i];
            const TRow& row_down_before = tmesh_before[i + 1];

            TRow& row_main = tmesh[i];

            // 恢复 main 行的控制点
            for (size_t j = 0; j < row_main.control_points.size(); ++j) {
                row_main.control_points[j] = row_main_before.control_points[j];
            }
        }

        // 3. 更新 down 控制点（插值）
        update_tmesh_Wij_down_with_interpolation(tmesh, v_knots, degree_v);
        //saveSurfaceToObjFile(tmesh, curves, v_knots, 3, 3, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface111_intter.obj");不贴合
        //printTMesh(tmesh);
        // 4. 计算当前能量
        double energy = computeUBendingEnergy_Gauss5(tmesh, curves, v_knots, degree_u, degree_v);
        double dE = energy - prev_energy;

        std::cout << "Iter " << iter
            << " | Energy: " << energy
            << " | dE: " << dE << std::endl;

        // 如果能量增加，停止迭代
        if (dE > 0) {
            std::cout << "Energy increased, stopping iteration at " << iter << std::endl;
            break;
        }

        // 如果能量变化小，认为收敛
        if (std::abs(dE) < tol) {
            std::cout << "Converged at iteration " << iter << std::endl;
            break;
        }

        prev_energy = energy;
    }
}
//--------------------------------------
// 自适应步长梯度下降优化 T-mesh 的 up 控制点
//--------------------------------------
void optimizeTMesh_UpGradientDescent_Adaptive(
    std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v,
    int max_iter = 100,
    double alpha_init = 1e-3,
    double tol = 1e-6,
    double alpha_increase = 1.05,  // 能量下降时步长放大比例
    double alpha_decrease = 0.5    // 能量上升时步长缩小比例
) {
    double prev_energy = computeUBendingEnergy_Gauss5(tmesh, curves, v_knots, degree_u, degree_v);
    double alpha = alpha_init;

    for (int iter = 0; iter < max_iter; ++iter) {
        // ---------- 1. 计算梯度 ----------
        auto grad_up = compute_dEu_dWij_up_matrix(tmesh, curves, v_knots, degree_u, degree_v);

        // ---------- 2. 备份当前 T-mesh ----------
        auto tmesh_backup = tmesh;

        // ---------- 3. 尝试更新 up 控制点 ----------
        update_tmesh_Wij_up(tmesh, grad_up, alpha);

        // ---------- 4. 更新 down 控制点 ----------
        update_tmesh_Wij_down_with_interpolation(tmesh, v_knots, degree_v);

        // ---------- 5. 计算新的能量 ----------
        double energy = computeUBendingEnergy_Gauss5(tmesh, curves, v_knots, degree_u, degree_v);

        // ---------- 6. 判断能量变化 ----------
        if (energy < prev_energy) {
            // 能量下降 → 增加步长略微加快收敛
            alpha *= alpha_increase;
            prev_energy = energy;
            std::cout << "Iter " << iter << " : Energy = " << energy
                << ", alpha = " << alpha << std::endl;
        }
        else {
            // 能量上升 → 恢复上一步 T-mesh，缩小步长
            tmesh = tmesh_backup;
            alpha *= alpha_decrease;
            std::cout << "Iter " << iter << " : Energy increased! Reduce alpha to " << alpha << std::endl;
        }

        // ---------- 7. 收敛判断 ----------
        if (std::abs(prev_energy - energy) < tol) {
            std::cout << "Converged at iteration " << iter << std::endl;
            break;
        }

        // 防止 alpha 太小陷入停滞
        if (alpha < 1e-12) {
            std::cout << "Alpha too small, stopping iteration." << std::endl;
            break;
        }
    }
}



Eigen::SparseMatrix<double>
computeKu_Gauss5(
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u,
    int degree_v
)
{
    using Triplet = Eigen::Triplet<double>;

    // 控制点总数（假设每个 TRow 有 global index）
    int nCtrl = 0;
    for (const auto& row : tmesh)
        nCtrl += static_cast<int>(row.control_points.size());

    std::vector<Triplet> triplets;

    // -------- 五点 Gauss --------
    const double xi[5] = {
        0.0,
       -0.5384693101056831,
        0.5384693101056831,
       -0.9061798459386640,
        0.9061798459386640
    };
    const double w[5] = {
        0.5688888888888889,
        0.4786286704993665,
        0.4786286704993665,
        0.2369268850561891,
        0.2369268850561891
    };

    // -------- Gauss 积分 --------
    for (int gi = 0; gi < 5; ++gi) {
        double u = 0.5 * (xi[gi] + 1.0);
        double wu = 0.5 * w[gi];

        for (int gj = 0; gj < 5; ++gj) {
            double v = 0.5 * (xi[gj] + 1.0);
            double wv = 0.5 * w[gj];

            double weight = wu * wv;

            // ---- 预计算 v 方向基函数 ----
            BasisResult M_vals =
                computeBasisAndDerivatives_ON(degree_v, v_knots, v);

            // 全局控制点 index
            int global_i = 0;

            // 遍历 TMesh 的每一行
            for (size_t i = 0; i < tmesh.size(); ++i) {

                const TRow& row = tmesh[i];

                double M = M_vals.N[i];
                double Mdd = M_vals.ddN[i];

                // 找对应的 u 方向 knot
                int orig_idx =
                    findOriginalCurveForInterpolatedRow(tmesh, i);
                const ON_NurbsCurve& orig_curve = curves[orig_idx];

                std::vector<double> u_knot;
                for (int k = 0; k < orig_curve.KnotCount(); ++k)
                    u_knot.push_back(orig_curve.Knot(k));

                BasisResult N_vals =
                    computeBasisAndDerivatives_ON(degree_u, u_knot, u);

                // 当前行内控制点
                for (size_t j = 0; j < row.control_points.size(); ++j) {

                    double Ndd = N_vals.ddN[j];
                    double Buu_i = Ndd * M;

                    int global_j = 0;

                    // 再来一遍，形成 (i,j)
                    for (size_t ii = 0; ii < tmesh.size(); ++ii) {

                        const TRow& row2 = tmesh[ii];

                        double M2 =
                            M_vals.N[ii];

                        int orig_idx2 =
                            findOriginalCurveForInterpolatedRow(tmesh, ii);
                        const ON_NurbsCurve& curve2 =
                            curves[orig_idx2];

                        std::vector<double> u_knot2;
                        for (int k = 0; k < curve2.KnotCount(); ++k)
                            u_knot2.push_back(curve2.Knot(k));

                        BasisResult N_vals2 =
                            computeBasisAndDerivatives_ON(degree_u, u_knot2, u);

                        for (size_t jj = 0; jj < row2.control_points.size(); ++jj) {

                            double Buu_j =
                                N_vals2.ddN[jj] * M2;

                            triplets.emplace_back(
                                global_i,
                                global_j,
                                weight * Buu_i * Buu_j
                            );

                            ++global_j;
                        }
                    }

                    ++global_i;
                }
            }
        }
    }

    Eigen::SparseMatrix<double> Ku(nCtrl, nCtrl);
    Ku.setFromTriplets(triplets.begin(), triplets.end());

    return Ku;
}


void exportKu(
    const Eigen::SparseMatrix<double>& Ku,
    const std::vector<TRow>& tmesh,
    const std::string& filename)
{
    // 打开文件
    std::ofstream fout(filename);

    int nCtrl = 0;
    for (const auto& row : tmesh)
        nCtrl += row.control_points.size();

    Eigen::MatrixXd Kdense = Eigen::MatrixXd(Ku);  // 转为稠密矩阵方便 MATLAB 读

    fout << nCtrl << std::endl;

    // 输出 Ku
    for (int i = 0; i < nCtrl; i++) {
        for (int j = 0; j < nCtrl; j++) {
            fout << Kdense(i, j);
            if (j < nCtrl - 1) fout << " ";
        }
        fout << "\n";
    }
    fout.close();
}

void exportCtrlPoints(
    const std::vector<TRow>& tmesh,
    const std::string& filename)
{
    // 打开文件
    std::ofstream fout(filename);
    // 输出控制点 xyz
    for (const auto& row : tmesh) {
        for (const auto& P : row.control_points) {
            fout << P.x << " " << P.y << " " << P.z << "\n";
        }
    }

    fout.close();
}

void printAndExportRowSizes(const std::vector<TRow>& tmesh, const std::string& filename)
{
    std::cout << "每排控制点数量：" << std::endl;

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Error: cannot open file " << filename << " for writing." << std::endl;
        return;
    }

    int total = 0;

    for (size_t i = 0; i < tmesh.size(); ++i) {
        int nCpt = static_cast<int>(tmesh[i].control_points.size());
        std::cout << "Row " << i + 1 << ": " << nCpt << std::endl;
        fout << nCpt << "\n";  // 每排一个数字
        total += nCpt;
    }

    std::cout << "总控制点数 = " << total << std::endl;
    fout.close();
}

void exportABCforDown(
    const std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,
    int degree_v,
    const std::string& filename
) {
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Cannot open file " << filename << std::endl;
        return;
    }

    size_t N = tmesh.size();

    // 遍历中间插入行，每组三行 (up, main, down)
    for (size_t i = 3; i < N - 1; i += 3) {
        const TRow& row_main = tmesh[i];
        double v_param = row_main.v;  // 当前 main 行对应的 v

        double a, b, c;
        std::tie(a, b, c) = computeVWeightsForRow(v_knots, degree_v, v_param, i);

        // 写入文件：每行一个主行 (i) 的系数 a b c
        fout << a << " " << b << " " << c << "\n";
    }

    fout.close();
    std::cout << "abcDown.txt exported." << std::endl;
}

// 从文件更新 tmesh 中的控制点
void importCtrlPointsToTmesh(std::vector<TRow>& tmesh, const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        throw std::runtime_error("无法打开文件 " + filename);
    }

    for (auto& row : tmesh) {
        for (auto& P : row.control_points) {
            std::string line;
            if (!std::getline(fin, line)) {
                throw std::runtime_error("文件中的控制点数量不足 tmesh 总数");
            }

            std::istringstream ss(line);
            ss >> P.x >> P.y >> P.z;
        }
    }

    // 检查文件是否还有多余控制点
    std::string dummy;
    if (std::getline(fin, dummy)) {
        std::cerr << "警告: 文件中存在多余的控制点，未使用\n";
    }

    fin.close();
}