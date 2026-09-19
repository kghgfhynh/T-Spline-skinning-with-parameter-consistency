///////////////////////////////////////T样条曲面蒙皮//////////////////////////////
#include "Ohmethod.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <iomanip>

//#include <Eigen/Dense>

std::vector<double> grevilleAbscissae(const ON_NurbsCurve& curve) {
    int p = curve.Degree();               // degree
    int n = curve.CVCount() - 1;          // 控制点个数-1
    int m = curve.KnotCount();            // 节点数

    std::vector<double> xi(n + 1, 0.0);
    const double* knots = curve.m_knot;

    for (int i = 0; i <= n; ++i) {
        double sum = 0.0;
        for (int j = 0; j < p; ++j) {
            sum += knots[i + j];
        }
        xi[i] = sum / p;
    }
    return xi;
}

ON_3dPoint evaluateNURBS(const ON_NurbsCurve& curve, double u) {
    ON_3dPoint pt;
    curve.Evaluate(u, 0, 3, &pt.x);
    return pt;
}
//随便给定插入曲线控制点
void initInsertedRow(TRow& row, const ON_NurbsCurve& c1, const ON_NurbsCurve& c2, bool isUp) {
    row.control_points.resize(row.u_positions.size());
    for (size_t j = 0; j < row.u_positions.size(); ++j) {
        double u = row.u_positions[j];
        ON_3dPoint p1 = evaluateNURBS(c1, u);
        ON_3dPoint p2 = evaluateNURBS(c2, u);
        if (isUp)
            row.control_points[j] = (2.0 * p1 + p2) / 3.0;
        else
            row.control_points[j] = (p1 + 2.0 * p2) / 3.0;
    }
}

std::vector<TRow> buildTMesh(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt)
{
    std::vector<TRow> tmesh;

    for (size_t i = 0; i < curves.size(); ++i) {
        // 原始行
        TRow row;
        row.v = para_v_firstcpt[i];
        row.u_positions = grevilleAbscissae(curves[i]);
        row.control_points.resize(curves[i].CVCount());
        for (int j = 0; j < curves[i].CVCount(); ++j) {
            ON_3dPoint cv;
            curves[i].GetCV(j, cv);
            row.control_points[j] = cv;
        }
        tmesh.push_back(row);

        // 插入行
        if (i < curves.size() - 1) {
            double v_up = (2.0 * para_v_firstcpt[i] + para_v_firstcpt[i + 1]) / 3.0;
            double v_down = (para_v_firstcpt[i] + 2.0 * para_v_firstcpt[i + 1]) / 3.0;

            TRow row_up, row_down;
            row_up.v = v_up;
            row_up.u_positions = grevilleAbscissae(curves[i]);
            initInsertedRow(row_up, curves[i], curves[i + 1], true);

            row_down.v = v_down;
            row_down.u_positions = grevilleAbscissae(curves[i + 1]);
            initInsertedRow(row_down, curves[i], curves[i + 1], false);

            tmesh.push_back(row_up);
            tmesh.push_back(row_down);
        }
    }
    return tmesh;
}


void printTMesh(const std::vector<TRow>& tmesh) {
    std::cout << "===== T-mesh 控制点 =====" << std::endl;
    for (size_t i = 0; i < tmesh.size(); ++i) {
        const TRow& row = tmesh[i];
        std::cout << "Row " << i << " (v=" << row.v << "):" << std::endl;

        // 打印该行的 u_positions
        std::cout << "   U positions: ";
        for (size_t j = 0; j < row.u_positions.size(); ++j) {
            std::cout << row.u_positions[j] << " ";
        }
        std::cout << std::endl;

        // 打印控制点
        for (size_t j = 0; j < row.control_points.size(); ++j) {
            const ON_3dPoint& pt = row.control_points[j];
            std::cout << "   P(" << j << ") = ("
                << pt.x << ", "
                << pt.y << ", "
                << pt.z << ")" << std::endl;
        }
    }
    std::cout << "=========================" << std::endl;
}

std::vector<double> knotsFromGreville(const std::vector<double>& greville, int degree) {
    int n = (int)greville.size() - 1;  // 控制点个数 - 1n=9
    int m = n + degree + 1;            // knot 最后索引m=13
    std::vector<double> knots(m - 1, 0.0);

    // 前端 clamped
    for (int i = 0; i < degree; i++)
        knots[i] = 0.0;
    // 后端 clamped
    for (int i = m - degree - 1; i < m - 1; i++)
        knots[i] = 1.0;

    // 中间 knots
    for (int j = 1; j <= n - degree; j++) {
        double sum = 0.0;
        for (int k = j; k <= j + degree - 1; k++) {
            sum += greville[k];
        }
        knots[j + degree - 1] = sum / degree;
    }

    return knots;
}
// helper: 从 tmesh 收集每一行的 v（保持 tmesh 的行顺序）
std::vector<double> collectRowVs(const std::vector<TRow>& tmesh) {
    std::vector<double> vs;
    vs.reserve(tmesh.size());
    for (const auto& row : tmesh) {
        vs.push_back(row.v);
    }
    // 如果你确信 buildTMesh 已按 v 单调递增，可以不排序；
    // 若不确定，可使用下面两行来排序并去重（注意：排序可能改变原本行序）：
    // std::sort(vs.begin(), vs.end());
    // vs.erase(std::unique(vs.begin(), vs.end()), vs.end());
    return vs;
}




// 修正后的 computeBasisFunctionsAt
std::vector<double> computeBasisFunctionsAt(
    int degree,                        // 次数 d
    const std::vector<double>& knots,  // OpenNURBS 风格 knot 向量 (长度 = n + d - 1)
    double t
) {
    // 计算控制点数量 n（OpenNURBS: knot.size() = n + d - 1）
    int n = (int)knots.size() - degree + 1;
    int order = degree + 1;

    std::vector<double> N_global(n, 0.0);

    // 找到 span k 满足 knots[k] <= t < knots[k+1], k in [d-1, n-1)
    int k = -1;
    for (int i = degree - 1; i < n; ++i) {
        if (t >= knots[i] && t < knots[i + 1]) {
            k = i;
            break;
        }
    }
    // 边界 t == knots[n-1] 的处理 (取最后一个控制点)
    if (k == -1) {
        if (fabs(t - knots[n - 1]) < 1e-12) {
            N_global[n - 1] = 1.0;
            return N_global;
        }
        // 不在定义域
        /*std::cerr << "computeBasisFunctionsAt: t=" << t << " 不在曲线定义域 ["
            << knots[degree - 1] << "," << knots[n - 1] << "] 内！" << std::endl;*/
        return N_global;
    }
    //std::cout << "k:" << k << std::endl;
    // 分配 order*order 数组，按 OpenNURBS 要求传入
    std::vector<double> N_local(order * order, 0.0);

    // ON_EvaluateNurbsBasis 要求传入活动区间的 knot 指针：k - degree + 1
    const double* knotPtr = &knots[k - degree + 1];
    bool ok = ON_EvaluateNurbsBasis(order, knotPtr, t, N_local.data());
    if (!ok) {
        std::cerr << "ON_EvaluateNurbsBasis 失败 t=" << t << std::endl;
        return N_global;
    }

    // 正确地取出 degree 阶基函数：它们位于 N_local 的前 order 个元素 (row 0)
    // 并映射到全局基函数索引 idx = (k - degree + 1) + m
    for (int m = 0; m < order; ++m) {
        int idx = k - degree + 1 + m;
        if (idx >= 0 && idx < n) {
            N_global[idx] = N_local[m]; // 注意：取 N_local[m] （第一行）
        }
    }

    return N_global;
}

// 输入：
//   v_knots  : OpenNURBS 风格的 V 方向节点向量
//   degree_v : V 方向次数 (p)
//   v_param  : 当前行的参数 v^i
//   i        : 当前控制点索引
// 返回 (a, b, c) = (M_{i-1}(v^i), M_i(v^i), M_{i+1}(v^i))
std::tuple<double, double, double> computeVWeightsForRow(
    const std::vector<double>& v_knots,
    int degree_v,
    double v_param,
    int i   // 当前行的基函数索引
) {
    // 计算所有基函数在 v_param 处的值
    std::vector<double> N_global = computeBasisFunctionsAt(degree_v, v_knots, v_param);
    //// 直接打印
    //std::cout << "Basis values at v=" << v_param << ": ";
    //for (size_t i = 0; i < N_global.size(); ++i) {
    //    std::cout << "N_" << i << "=" << N_global[i] << " ";
    //}
    //std::cout << std::endl;
    double a = 0.0, b = 0.0, c = 0.0;

    if (i - 1 >= 0) {
        a = N_global[i - 1];  // M_{i-1}(v^i)
    }
    if (i >= 0 && i < (int)N_global.size()) {
        b = N_global[i];      // M_i(v^i)
    }
    if (i + 1 < (int)N_global.size()) {
        c = N_global[i + 1];  // M_{i+1}(v^i)
    }

    return { a, b, c };
}

// 测试在一组参数点下的基函数值矩阵
void testBasisMatrix(const std::vector<double>& knots, int degree,
    const std::vector<double>& params)
{
    int n = (int)knots.size() - degree + 1; // 控制点数
    /*std::cout << "==== Basis Matrix (degree=" << degree
        << ", n=" << n << ") ====" << std::endl;*/

    for (size_t pi = 0; pi < params.size(); ++pi) {
        double t = params[pi];
        std::vector<double> N = computeBasisFunctionsAt(degree, knots, t);

        /*std::cout << "t=" << t << " : ";
        for (int i = 0; i < n; ++i) {
            std::cout << std::fixed << std::setprecision(6) << N[i] << " ";
        }
        std::cout << std::endl;*/
    }
    //std::cout << "=================================" << std::endl;
}

// 更新T-mesh中的控制点，应用插值条件
void updateControlPointsWithInterpolation(
    std::vector<TRow>& tmesh,
    const std::vector<double>& v_knots,  // V方向的公共节点向量
    int degree_v
) {
    // 遍历每一行
    for (size_t i = 3; i < tmesh.size() - 1; i += 3) { // 跳过第一条和最后一条曲线
        TRow& row_main = tmesh[i];
        const TRow& row_up = tmesh[i - 1];   // 上一行
        const TRow& row_down = tmesh[i + 1]; // 下一行


        // 获取每个控制点对应的参数 v
        double v_param = row_main.v;
        //std::cout << "v_param:" << v_param << std::endl;
        // 计算 a, b, c
        double a, b, c;
        std::tie(a, b, c) = computeVWeightsForRow(v_knots, degree_v, v_param, i);
        //std::cout << " -> a=" << a << " b=" << b << " c=" << c << std::endl;
        // 更新 Wi
        for (size_t j = 0; j < row_main.control_points.size(); ++j) {
            ON_3dPoint& Wi = row_main.control_points[j];

            // 获取 Wi 对应的权重
            ON_3dPoint Wi_up = row_up.control_points[j];
            ON_3dPoint Wi_down = row_down.control_points[j];

            // 应用插值公式更新控制点
            Wi = (Wi - (a * Wi_up + c * Wi_down)) / b;
        }

    }
}
int findOriginalCurveForInterpolatedRow(const std::vector<TRow>& tmesh, int row_index) {
    const TRow& interpolated_row = tmesh[row_index];


    // 获取原始曲线数量
    int n = (tmesh.size() - 4) / 3; // 假设每三个行是一组插值
    int original_curve_index = 0;

    // 判断插值曲线对应的原始曲线
    if (row_index == 0) {
        // 第1条插值曲线对应原始的第1条曲线
        original_curve_index = 0;
    }
    else if (row_index == tmesh.size() - 1) {
        // 最后一条插值曲线对应原始的第n条曲线
        original_curve_index = n + 2 - 1;
    }
    else if (row_index == tmesh.size() - 2) {
        // 最后一条插值曲线对应原始的第n条曲线
        original_curve_index = n + 2 - 1;
    }
    else {
        // 中间的插值曲线根据公式判断其原始曲线

        for (int j = 1; j <= n; j++) {
            if (row_index >= (2 + 3 * (j - 1)) && row_index < (2 + 3 * j)) {
                original_curve_index = j;
                break;
            }
        }
    }
    //std::cout << "idx  = " << row_index << " origin " << original_curve_index << std::endl;
    return original_curve_index;

}

// 计算T样条曲面上的一个点 S(u, v)
ON_3dPoint computeSurfacePoint(
    double u, double v,                      // 参数 u, v
    const std::vector<TRow>& tmesh,          // TMesh 数据
    const std::vector<ON_NurbsCurve>& curves,//曲线数据，提供u方向上节点向量
    const std::vector<double>& v_knots,      // V方向节点向量
    int degree_u,                            // U方向次数
    int degree_v                             // V方向次数
) {
    ON_3dPoint surface_point(0.0, 0.0, 0.0); // 初始化曲面点
    double weight_sum = 0.0;                 // 权重和

    // 遍历 TMesh 的每一行（即每个 v 值对应的曲线）
    for (size_t i = 0; i < tmesh.size(); ++i) {
        const TRow& row = tmesh[i];
        //double v = row.v; // 当前行的 v 值

        // 计算当前行的基函数 M(v)
        std::vector<double> M_v_values = computeBasisFunctionsAt(degree_u, v_knots, v);
        double M_v = M_v_values[i];

        // 查找对应的原始曲线索引
        int original_curve_index = findOriginalCurveForInterpolatedRow(tmesh, i);
        // 获取当前曲线的控制点与权重
        const ON_NurbsCurve& original_curve = curves[original_curve_index];  // 获取原始曲线
        std::vector<double>u_knot;
        for (size_t num = 0; num < original_curve.KnotCount(); ++num) {
            double kt;
            u_knot.push_back(original_curve.Knot(num));
        }
        /*std::cout << "u_knot: ";
        for (size_t i = 0; i < u_knot.size(); ++i) {
            std::cout << u_knot[i] << " ";
        }
        std::cout << std::endl;*/
        // 获取当前曲线的控制点与权重
        std::vector<double> N_u_values = computeBasisFunctionsAt(degree_u, u_knot, u);


        // 获取当前曲线的控制点与权重
        for (size_t j = 0; j < row.control_points.size(); ++j) {
            // 对应控制点 P_ij 的基函数值
            double N_u = N_u_values[j];

            // 权重
            double weight = 1; // M(v)的权重

            //// 累加权重与控制点的加权和
            //std::cout << "*" << "M_v= " << M_v << " ; " << "N_u=" << N_u << "*" << std::endl;

            const ON_3dPoint& pt = row.control_points[j];

            surface_point += pt * (M_v * N_u * weight);

            weight_sum += weight * N_u * M_v;


        }
    }
    /*std::cout << "u=" << u << ";;" << "v=" << v << std::endl;
    std::cout << "surface_point " << ": ("
        << surface_point.x << ", "
        << surface_point.y << ", "
        << surface_point.z << ")" << "weight=" <<weight_sum<< std::endl;*/

        //std::cout << "weight_sum:" << weight_sum << std::endl;
        // 归一化
    surface_point /= weight_sum;
    // 输出 u, v 对应的曲面点
    /*std::cout << "Surface point at u = " << u << ", v = " << v << ": ("
        << surface_point.x << ", "
        << surface_point.y << ", "
        << surface_point.z << ")" << std::endl;
    std::cout << "=============" << std::endl;*/
    return surface_point;
}


// 保存T样条曲面到obj文件
void saveSurfaceToObjFile(
    const std::vector<TRow>& tmesh,          // TMesh 数据
    const std::vector<ON_NurbsCurve>& curves,// 曲线数据
    const std::vector<double>& v_knots,      // V方向节点向量
    int degree_u,                            // U方向次数
    int degree_v,                            // V方向次数
    double u_min, double u_max,              // U方向的参数范围
    double v_min, double v_max,              // V方向的参数范围
    int u_samples, int v_samples,            // U, V方向的采样数量
    const std::string& filename              // 输出的文件名
) {
    // 步长
    double u_step = (u_max - u_min) / (u_samples - 1);
    double v_step = (v_max - v_min) / (v_samples - 1);

    // 存储顶点和面
    std::vector<ON_3dPoint> vertices;
    std::vector<std::vector<int>> faces;

    // 计算曲面上的所有顶点
    for (int j = 0; j < v_samples; ++j) {
        for (int i = 0; i < u_samples; ++i) {
            double u = u_min + i * u_step;
            double v = v_min + j * v_step;

            // 计算T样条曲面上的点
            ON_3dPoint surface_point = computeSurfacePoint(u, v, tmesh, curves, v_knots, degree_u, degree_v);
            vertices.push_back(surface_point);
        }
    }

    // 构建面
    for (int i = 0; i < u_samples-1 ; ++i) {
        for (int j = 0; j < v_samples-1 ; ++j) {
            int index1 = i * v_samples + j + 1; // 1-based index for OBJ format
            int index2 = (i + 1) * v_samples + j + 1;
            int index3 = (i + 1) * v_samples + (j + 1) + 1;
            int index4 = i * v_samples + (j + 1) + 1;

            // 添加面
            faces.push_back({ index1, index4, index3, index2 });
            //faces.push_back({ index1, index2, index3, index4 });
        }
    }
    
    // 保存到obj文件
    std::ofstream obj_file(filename);
    if (!obj_file.is_open()) {
        std::cerr << "can't open file" << filename << std::endl;
        return;
    }

    // 写入顶点
    for (const auto& vertex : vertices) {
        obj_file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }

    // 写入面（四边形）
    for (const auto& face : faces) {
        obj_file << "f";
        for (int idx : face) {
            obj_file << " " << idx;
        }
        obj_file << std::endl;
    }


    obj_file.close();
    std::cout << "tspline saved: " << filename << std::endl;
}


