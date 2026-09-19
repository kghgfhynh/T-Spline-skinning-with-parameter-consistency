#include "paramatch.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <iomanip>
#include <iostream>
//生成均匀节点向量
std::vector<double> generateUniformKnots(int cv_count, int degree)
{
    int knot_count = cv_count + degree - 1;
    std::vector<double> knots(knot_count);

    int inner_count = knot_count - 2 * degree;

    for (int i = 0; i < degree; i++)
        knots[i] = 0.0;

    for (int i = 0; i < inner_count; i++)
        knots[degree + i] = double(i + 1) / (inner_count + 1);

    for (int i = knot_count - degree; i < knot_count; i++)
        knots[i] = 1.0;

    return knots;
}
// 弦长参数化，返回参数化数组u
std::vector<double> chordLengthParam(std::vector<ON_3dPoint>& points) {
    std::vector<double> u(points.size(), 0.0);
    if (points.size() < 2) return u;

    double totalLength = 0.0;
    for (size_t i = 1; i < points.size(); ++i) {
        double dist = (points[i] - points[i - 1]).Length();
        totalLength += dist;
        u[i] = totalLength;
    }
    // 归一化到[0,1]
    for (size_t i = 1; i < points.size(); ++i) {
        u[i] /= totalLength;
    }
    return u;
}

ON_NurbsCurve CreateNURBSCurve(
    int dimension,
    bool isRational,
    int order,
    const std::vector<ON_3dPoint>& controlPoints,
    const std::vector<double>& weights,
    const std::vector<double>& knots
) {
    ON_NurbsCurve curve;  // 局部对象，非指针

    // 参数校验（部分示例）
    if (dimension < 1 || order < 2 || controlPoints.empty()) {
        std::cerr << "Invalid input parameters." << std::endl;
        return curve;  // 返回空对象
    }
    int cvCount = static_cast<int>(controlPoints.size());
    if (isRational && weights.size() != static_cast<size_t>(cvCount)) {
        std::cerr << "Invalid weights size." << std::endl;
        return curve;
    }
    if (knots.size() < static_cast<size_t>(order + cvCount - 2)) {
        std::cerr << "Invalid knots size." << std::endl;
        return curve;
    }

    // 创建曲线
    if (!curve.Create(dimension, isRational, order, cvCount)) {
        std::cerr << "Create failed." << std::endl;
        return curve;
    }

    // 设置控制点和权重
    for (int i = 0; i < cvCount; ++i) {
        curve.SetCV(i, controlPoints[i]);
        if (isRational) {
            curve.SetWeight(i, weights[i]);
        }
    }
    // 设置节点向量
    for (size_t i = 0; i < knots.size(); ++i) {
        curve.SetKnot(i, knots[i]);
    }

    if (!curve.IsValid()) {
        std::cerr << "Curve not valid." << std::endl;
        return ON_NurbsCurve();  // 返回空对象
    }

    return curve;  // 返回对象，非指针
}

// 输出单条NURBS曲线的阶数、控制点数量、控制点、节点向量及权重
void PrintNURBSCurveInfo(const ON_NurbsCurve& curve) {
    // 1. 输出曲线阶数（Degree）
    int degree = curve.Degree();
    std::cout << "=== NURBS ===" << std::endl;
    std::cout << "Degree: " << degree << std::endl;

    // 2. 输出控制点数量（CV Count）
    int cv_count = curve.CVCount();
    std::cout << "CVnum: " << cv_count << std::endl;

    // 3. 输出控制点坐标（3D点）
    std::cout << "CV:" << std::endl;
    for (int i = 0; i < cv_count; ++i) {
        ON_3dPoint pt;
        curve.GetCV(i, pt); // 修正：逐个获取控制点坐标
        std::cout << "  cpt" << i << ": (" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
    }

    // 4. 输出节点向量（Knot Vector）
    std::cout << "Knot Vector:" << std::endl;
    int knot_count = curve.KnotCount();
    const double* knots = curve.Knot(); // 获取节点数组指针
    for (int i = 0; i < knot_count; ++i) {
        std::cout << "knot" << i << ": " << knots[i] << std::endl;
    }

    // 5. 输出权重（若有理）
    if (curve.IsRational()) {
        std::cout << "Weights:" << std::endl;
        for (int i = 0; i < cv_count; ++i) {
            double weight;
            weight = curve.Weight(i); // 修正：直接获取权重值
            std::cout << "CV" << i << "weight: " << weight << std::endl;
        }
    }
    else {
        std::cout << "is not rational" << std::endl;
    }
}

// 采样一条NURBS曲线的点集，等参数间隔采样,这个采样只是为了看出连线的扭曲
std::vector<ON_3dPoint> sampleCurve(const ON_NurbsCurve& curve, int num_samples) {
    std::vector<ON_3dPoint> points;

    // 取参数范围，NURBS有效区间是knots[degree]到knots[numCVs]
    int degree = curve.Degree();
    int numCVs = curve.CVCount();
    const double* knots = curve.m_knot;
    // 计算节点总数
    unsigned knotCount = curve.KnotCount();
    //std::cout << " knotCount" << knotCount << std::endl;
    double u_start = knots[degree-1];
    double u_end = knots[knotCount - degree];
    //std::cout << "uend:" << u_end << std::endl;
    for (int i = 0; i < num_samples; ++i) {
        double u = u_start + (u_end - u_start) * double(i) / (num_samples - 1);
        ON_3dPoint pt;
        curve.Evaluate(u,0,3,&pt.x);
        points.push_back(pt);
        //// 增加打印，显示当前参数及对应点坐标
        //std::cout << "u = " << u
        //    << ", pt = (" << pt.x << ", " << pt.y << ", " << pt.z << ")"
        //    << std::endl;
    }
    return points;
}
//均分弧长采样
std::vector<ON_3dPoint> sampleCurveByArcLength(
    const ON_NurbsCurve& curve,
    int num_samples)
{
    std::vector<ON_3dPoint> result;

    if (num_samples < 2)
        return result;

    int degree = curve.Degree();
    unsigned knotCount = curve.KnotCount();
    const double* knots = curve.m_knot;

    double u_start = knots[degree - 1];
    double u_end = knots[knotCount - degree];

    // ===== Step 1: 高密度离散曲线 =====
    int dense_samples = 1000;

    std::vector<double> u_table(dense_samples);
    std::vector<ON_3dPoint> dense_pts(dense_samples);
    std::vector<double> arc_table(dense_samples, 0.0);

    for (int i = 0; i < dense_samples; ++i)
    {
        double t = double(i) / (dense_samples - 1);
        double u = u_start + t * (u_end - u_start);

        u_table[i] = u;
        curve.Evaluate(u, 0, 3, &dense_pts[i].x);

        if (i > 0)
        {
            arc_table[i] = arc_table[i - 1] +
                dense_pts[i].DistanceTo(dense_pts[i - 1]);
        }
    }

    double total_length = arc_table.back();

    // ===== Step 2: 等弧长采样 =====
    for (int k = 0; k < num_samples; ++k)
    {
        double target_length =
            total_length * double(k) / (num_samples - 1);

        // 找到弧长所在区间
        auto it = std::lower_bound(
            arc_table.begin(),
            arc_table.end(),
            target_length);

        int idx = std::distance(arc_table.begin(), it);

        if (idx == 0)
        {
            result.push_back(dense_pts[0]);
            continue;
        }

        if (idx >= dense_samples)
        {
            result.push_back(dense_pts.back());
            continue;
        }

        // 线性插值反求参数
        double s1 = arc_table[idx - 1];
        double s2 = arc_table[idx];

        double ratio = (target_length - s1) / (s2 - s1);

        double u =
            u_table[idx - 1] +
            ratio * (u_table[idx] - u_table[idx - 1]);

        ON_3dPoint pt;
        curve.Evaluate(u, 0, 3, &pt.x);

        result.push_back(pt);
    }

    return result;
}

// 写多条曲线顶点和折线，支持多条曲线之间横向连线
// crossLinesBetweenCurves为true时，写相邻曲线间参数对应点的横线连接线段
void writeMultiCurvesWithCrossLinesToOBJ(
    const std::vector<std::vector<ON_3dPoint>>& curves_points,
    const std::string& filename,
    bool crossLinesBetweenCurves )
{
    if (curves_points.empty()) {
        std::cerr << "No curve data" << std::endl;
        return;
    }

    // 验证所有曲线采样点数量相同，以便横向连线
    size_t sampleCount = curves_points[0].size();
    //for (const auto& pts : curves_points) {
    //    if (pts.size() != sampleCount) {
    //        std::cout << "The number of sampling points is inconsistent across all curves" << std::endl;
    //        return;
    //    }
    //}

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Unable to open file writes: " << filename << std::endl;
        return;
    }

    // 1. 写入所有顶点 （所有曲线顶点按序排放）
    for (const auto& curve : curves_points) {
        for (const auto& p : curve) {
            fout << "v " << p.x << " " << p.y << " " << p.z << "\n";
        }
    }

    size_t vertexOffset = 0;
    for (size_t c = 0; c < curves_points.size(); ++c) {
        const auto& curve = curves_points[c];
        if (curve.size() < 2) {
            vertexOffset += curve.size();
            continue;
        }

        // 分段写入折线（每段两个顶点作为独立折线）
        for (size_t i = 0; i < curve.size() - 1; ++i) {
            fout << "l " << (vertexOffset + i + 1) << " " << (vertexOffset + i + 2) << "\n";
        }

        vertexOffset += curve.size();
    }


    // 3. 写多条曲线之间的横向连接线，连接相邻两条曲线对应采样点
    if (crossLinesBetweenCurves && curves_points.size() >= 2) {
        size_t totalVerticesPerCurve = sampleCount;
        // 用多条横线连接所有相邻曲线对
        for (size_t c = 0; c < curves_points.size() - 1; ++c) {
            size_t offset1 = c * totalVerticesPerCurve;
            size_t offset2 = (c + 1) * totalVerticesPerCurve;

            for (size_t i = 0; i < sampleCount; ++i) {
                fout << "l " << (offset1 + i + 1) << " " << (offset2 + i + 1) << "\n";
            }
        }
    }

    fout.close();
    std::cout << " successfully written to the OBJ file: " << filename << std::endl;
}


std::vector<ON_NurbsCurve> SplitNurbsBySpans(const ON_NurbsCurve& curve)
{
    std::vector<ON_NurbsCurve> segments;

    int degree = curve.Degree();
    int knotCount = curve.KnotCount();

    if (knotCount < 2 * degree)
        return segments; // 不合法的曲线

    // 收集唯一的内部 knots
    std::vector<double> spanKnots;
    spanKnots.push_back(curve.Knot(degree - 1));                 // start
    for (int i = degree; i < knotCount - degree; ++i)
    {
        double k = curve.Knot(i);
        if (spanKnots.empty() || fabs(spanKnots.back() - k) > ON_SQRT_EPSILON)
        {
            spanKnots.push_back(k);
        }
    }
    spanKnots.push_back(curve.Knot(knotCount - degree));     // end

    // 遍历相邻 knot 作为区间，切出子曲线
    for (size_t i = 0; i + 1 < spanKnots.size(); ++i)
    {
        ON_Interval subdomain(spanKnots[i], spanKnots[i + 1]);//用相邻的两个 knot 值构造一个区间 subdomain，表示要切出来的曲线的参数范围。
        if (!subdomain.IsIncreasing())//检查区间是否合法：如果区间不是递增的（可能重复、退化），就跳过。
            continue;

        ON_NurbsCurve* dup = curve.DuplicateCurve()->NurbsCurve();//拷贝一份当前整条 NURBS 曲线（避免直接修改原始曲线），并把它转成 ON_NurbsCurve*
        if (dup && dup->Trim(subdomain))// 调用 Trim(subdomain) 把曲线裁剪到指定的参数范围
        {
            ON_NurbsCurve* seg = dup->NurbsCurve(); //把刚才 Trim 得到的 dup（类型是 ON_Curve*）转换为 ON_NurbsCurve*。
            if (seg)
            {
                segments.push_back(*seg); // 拷贝到 vector
            }
        }
        delete dup; // 避免内存泄漏
    }

    return segments;
}

void PrintNurbsSegments(const std::vector<ON_NurbsCurve>& segs)
{
    std::cout << "Total NURBS segments: " << segs.size() << std::endl;
    for (size_t i = 0; i < segs.size(); ++i)
    {
        const ON_NurbsCurve& c = segs[i];
        ON_Interval dom = c.Domain(); // 得到参数范围

        std::cout << "Segment " << i
            << " | Degree: " << c.Degree()
            << " | CV count: " << c.CVCount()
            << " | Domain: [" << dom.Min() << ", " << dom.Max() << "]"
            << std::endl;

        for (int j = 0; j < c.CVCount(); ++j)
        {
            ON_3dPoint pt;
            c.GetCV(j, pt);
            std::cout << "   CV[" << j << "] = ("
                << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
        }
    }
}

// 估算二阶导数上界
double EstimateSecondDerivativeBound(const ON_NurbsCurve & c, int sampleN)
{
    double maxVal = 0.0;
    ON_3dVector d1, d2;
    double t0 = c.Domain().Min();
    double t1 = c.Domain().Max();

    for (int i = 0; i <= sampleN; ++i)
    {
        double t = t0 + (t1 - t0) * i / sampleN;
        double v[3 * (2 + 1)]; // 3维 * (0阶+1阶+2阶) = 9 doubles
        if (c.Evaluate(t, 2, 3, v))
        {
            ON_3dPoint pt(v[0], v[1], v[2]);       // 曲线点
            ON_3dVector d1(v[3], v[4], v[5]);      // 一阶导
            ON_3dVector d2(v[6], v[7], v[8]);      // 二阶导

            double val = d2.Length(); // 这里直接取模作为近似上界
            if (val > maxVal) maxVal = val;
        }

    }
    return maxVal;
}
// 在一段 NURBS 曲线上采样点
std::vector<ON_3dPoint> SampleNurbsSegment(
    const ON_NurbsCurve& c, double epsilon)
{
    int p = c.Degree();
    double Buu = EstimateSecondDerivativeBound(c,50);
    int r = max(p, (int)std::ceil(std::sqrt((Buu / (8.0 * epsilon)))));
    if (r < 2) r = 2; // 至少两个点

    std::vector<ON_3dPoint> pts;
    double t0 = c.Domain().Min();
    double t1 = c.Domain().Max();

    for (int i = 0; i < r; ++i)
    {
        double t = t0 + (t1 - t0) * i / (r - 1);
        ON_3dPoint pt;
        if (c.Evaluate(t, 0, 3, &pt.x)) // 0 = just the point
            pts.push_back(pt);
    }
    return pts;
}

std::vector<ON_3dPoint>SampleCurveBySpans(const ON_NurbsCurve& curve, double epsilon)
{
    std::vector<ON_3dPoint> allPoints;

    // 先分段
    auto segments = SplitNurbsBySpans(curve);

    for (size_t i = 0; i < segments.size(); ++i)
    {
        auto pts = SampleNurbsSegment(segments[i], epsilon);

        // 避免重复端点：从第二段开始，跳过第一个点
        if (i > 0 && !pts.empty())
        {
            pts.erase(pts.begin());
        }

        // 追加到总结果
        allPoints.insert(allPoints.end(), pts.begin(), pts.end());
    }

    return allPoints;
}

//保存参数文件
void saveParametersToFile(const std::vector<double>& params, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& val : params) {
        outfile << val << "\n";  // 每个参数写一行
    }

    outfile.close();
}
//保存采样点到文件中用于matlab
void savePointToFile(const std::vector<ON_3dPoint>& points, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    // 每行写入一个点的坐标 (x, y, z)
    for (const auto& pt : points) {
        outfile << pt.x << " " << pt.y << " " << pt.z << "\n";
    }

    outfile.close();
}
// 平均节点法生成节点向量
std::vector<double> averageKnotsvector(const std::vector<double>& parameters, int degree, int numCtrlPts)
{
    int n = numCtrlPts - 1;                 // 控制点索引最大值
    int m = n + degree + 1-2;                 // 节点索引最大值
    std::vector<double> knots(m + 1, 0.0);  // 节点向量大小 m+1

    // 前 p+1 个 = 0
    for (int i = 0; i < degree; ++i)
        knots[i] = 0.0;

    // 后 p+1 个 = 1
    for (int i = m - degree+1; i <= m; ++i)
        knots[i] = 1.0;

    // 内部节点: j = 1..n-p
    for (int j = 1; j <= n - degree; ++j)
    {
        double sum = 0.0;
        for (int i = j; i < j + degree; ++i)
        {
            sum += parameters[i];
        }
        knots[j + degree-1] = sum / degree;
    }

    return knots;
}

// 生成均匀开放B样条节点向量
std::vector<double> uniformKnotVector(int numCtrlPts, int degree)
{
    int n = numCtrlPts - 1;             // 控制点最大下标
    int m = n + degree + 1-2;             // 节点最大下标
    std::vector<double> knots(m + 1, 0.0);

    // 前 degree+1 个置 0
    for (int i = 0; i < degree; ++i)
        knots[i] = 0.0;

    // 后 degree+1 个置 1
    for (int i = m - degree+1; i <= m; ++i)
        knots[i] = 1.0;

    // 中间节点均匀分布
    int numInterior = m - 2 * degree+1; // 内部节点个数
    for (int j = 1; j <= numInterior; ++j)
    {
        knots[degree + j-1] = (double)j / (numInterior + 1);
    }

    return knots;
}


void SolveControlPoints_OpenNurbs(
    int degree,                                       // 曲线次数-1，即degree=p
    const std::vector<double>& knotVector,           // 该NURBS的节点向量（非重复端点）
    const std::vector<double>& params,                // 参数点(u_0,u_1,...),每个对应一个空间点
    const std::vector<ON_3dPoint>& points,            // 空间点序列，对应参数点的曲线点坐标Y
    std::vector<ON_3dPoint>& controlPoints            // 输出的控制点序列Q，反求得
) {
    int order = degree + 1;
    int n = static_cast<int>(knotVector.size() + 2) - order - 1;  // 控制点数量-1
    //std::cout << "n:" << n << std::endl;
    int m = static_cast<int>(params.size());                  // 参数点数
    //std::cout << "m:" << m << std::endl;
    Eigen::MatrixXd N_mat(m, n + 1);// 创建矩阵N，大小m行(n+1)列，存储基函数值
    std::vector<double> N(order * order, 0.0);// 临时数组N，容量order*order，用于存放ON_EvaluateNurbsBasis计算结果

    for (int i = 0; i < m; ++i) {
        double u = params[i];
        // 找合适的节点段索引k，使 knotVector[k] <= u < knotVector[k+1]

        // 查找u所在的节点区间k，使knotVector[k] <= u < knotVector[k+1]
        int k = 0;
        for (; k < (int)knotVector.size() - 1; ++k) {
            if (u >= knotVector[k] && u < knotVector[k + 1]) break;
        }
        //std::cout << "u = " << u << ", corresponding span index k = " << k << std::endl;
        // 获取当前跨度的节点段首地址，长度为2*degree，用于计算基函数
        const double* knot_segment = &knotVector[k - order + 1 + 1];

        // 调用OpenNURBS的基函数计算函数，传入阶次、节点段、参数u和输出数组N
        bool ret = ON_EvaluateNurbsBasis(order, knot_segment, u, N.data());
        if (!ret) {
            std::cerr << "ON_EvaluateNurbsBasis failed at u=" << u << std::endl;
            continue;
        }

        // 填充N矩阵一行：N[d-k][i] = i-th degree k basis function at u
        // 其中 N[0..degree] 为 degree=p 的基函数值，对应当前段控制点
        int startIndex = k - degree + 1;
        for (int j = 0; j <= n; ++j) {
            // 理论上，只有 j 在 [startIndex, startIndex+degree] 范围内有非零基函数
            if (j >= startIndex && j <= startIndex + degree) {
                int bIndex = j - startIndex;
                if (bIndex >= 0 && bIndex < order) {
                    N_mat(i, j) = N[bIndex];
                }
                else {
                    N_mat(i, j) = 0.0;
                }
            }
            else {
                N_mat(i, j) = 0.0;
            }
        }
    }
    //// N_mat: Eigen::MatrixXd (m x (n+1))
    //Eigen::JacobiSVD<Eigen::MatrixXd> svd(N_mat, Eigen::ComputeThinU | Eigen::ComputeThinV);
    //double cond = svd.singularValues()(0) / svd.singularValues()(svd.singularValues().size() - 1);
    //std::cout << "cond(N_mat) = " << cond << std::endl;

    // //输出基函数矩阵
 /*   std::cout << "Basis function matrix N:" << std::endl;
    std::cout << N_mat << std::endl;*/
     //构造Y矩阵 (m行3列)
    Eigen::MatrixXd Y_mat(m, 3);
    for (int i = 0; i < m; ++i) {
        Y_mat(i, 0) = points[i].x;
        Y_mat(i, 1) = points[i].y;
        Y_mat(i, 2) = points[i].z;
    }
    ////输出基函数矩阵
    //std::cout << "Basis function matrix Y:" << std::endl;
    //std::cout << Y_mat << std::endl;
    // 只有j在[startIndex, startIndex+degree]区间内时基函数非零，赋对应值
    Eigen::MatrixXd Q_mat = N_mat.colPivHouseholderQr().solve(Y_mat);
    //// double lambda = 1e-8;
    ////Eigen::MatrixXd I = Eigen::MatrixXd::Identity(n + 1, n + 1);
    ////Eigen::MatrixXd Q_mat = (N_mat.transpose() * N_mat + lambda * I).ldlt().solve(N_mat.transpose() * Y_mat);
    //double lambda = 1e-6;
    //Eigen::MatrixXd I = Eigen::MatrixXd::Identity(n + 1, n + 1);
    //Eigen::MatrixXd Q_mat = (N_mat.transpose() * N_mat + lambda * lambda * I).ldlt().solve(N_mat.transpose() * Y_mat);
    //////输出基函数矩阵
    //std::cout << "Basis function matrix Q:" << std::endl;
    //std::cout << Q_mat << std::endl;
    // 转换结果到控制点数组
    controlPoints.resize(n + 1);
    for (int i = 0; i <= n; ++i) {
        controlPoints[i] = ON_3dPoint(Q_mat(i, 0), Q_mat(i, 1), Q_mat(i, 2));
    }
}

std::vector<double> readFileSkipFirstLast(const std::string& filename)
{
    std::vector<double> values;
    std::ifstream infile(filename);

    if (!infile.is_open())
    {
        std::cerr << "❌ 无法打开文件: " << filename << std::endl;
        return values;
    }

    double val;
    while (infile >> val)
    {
        values.push_back(val);
    }
    infile.close();

    // 文件中元素太少时直接返回空
    if (values.size() <= 2)
    {
        std::cerr << "⚠️ 文件 " << filename << " 中数据不足，跳过首尾后为空。" << std::endl;
        return std::vector<double>();
    }

    // 跳过首尾
    return std::vector<double>(values.begin() + 1, values.end() - 1);
}
