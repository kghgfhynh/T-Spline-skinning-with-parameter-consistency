#include "CGTS.h"


// ------------------------------------------------------------
// 计算单条曲线在 [v1, v2] 的弧长
// ------------------------------------------------------------
double ComputeLocalArcLength(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    int sample
)
{
    double length = 0.0;

    ON_3dPoint prev = curve.PointAt(v1);

    for (int i = 1; i <= sample; ++i)
    {
        double t = v1 + (v2 - v1) * double(i) / sample;

        ON_3dPoint curr = curve.PointAt(t);

        length += prev.DistanceTo(curr);

        prev = curr;
    }

    return length;
}
double FindParameterByArcLength(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    double targetLength,
    int samples
)
{
    double accumulated = 0.0;

    ON_3dPoint prev = curve.PointAt(v1);

    for (int i = 1; i <= samples; ++i)
    {
        double t = v1 + (v2 - v1) * double(i) / samples;

        ON_3dPoint curr = curve.PointAt(t);

        double segLen = prev.DistanceTo(curr);

        if (accumulated + segLen >= targetLength)
        {
            double ratio =
                (targetLength - accumulated) / segLen;

            return (t - (v2 - v1) / samples)
                + ratio * ((v2 - v1) / samples);
        }

        accumulated += segLen;
        prev = curr;
    }

    return v2;  // fallback
}
std::vector<double> ComputeGeometricUniformV_Local(
    const std::vector<ON_NurbsCurve>& guideCurves,
    double v1,
    double v2,
    int insertCount
)
{
    std::vector<double> result;

    if (insertCount <= 0 || guideCurves.empty())
        return result;

    int n = guideCurves.size();

    // 1️⃣ 计算局部平均弧长
    double avgLength = 0.0;

    for (int i = 0; i < n; ++i)
    {
        avgLength += ComputeLocalArcLength(
            guideCurves[i],
            v1,
            v2
        );
    }

    avgLength /= n;

    // 2️⃣ 用第一条引导线反求参数
    for (int k = 1; k <= insertCount; ++k)
    {
        double target =
            avgLength * double(k) / (insertCount + 1);

        double v_new =
            FindParameterByArcLength(
                guideCurves[0],
                v1,
                v2,
                target
            );

        result.push_back(v_new);
    }

    return result;
}







































// 输入：每条曲线的等参数采样点
// 输出：每条形状引导线的采样点集合
void SaveControlPointsToOBJ(const std::string& filename,
    const std::vector<ON_3dPoint>& ctrlPts)
{
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "无法打开 OBJ 文件: " << filename << std::endl;
        return;
    }

    fout << "# Control points saved as OBJ\n";

    // 写出顶点
    for (const auto& P : ctrlPts)
    {
        fout << "v " << P.x << " " << P.y << " " << P.z << "\n";
    }

   

    fout.close();
    std::cout << "控制点已保存到: " << filename << std::endl;
}

std::vector<std::vector<ON_3dPoint>> Transpose(
    const std::vector<std::vector<ON_3dPoint>>& samplestoguide)
{
    std::vector<std::vector<ON_3dPoint>> guidelinePoints;

    if (samplestoguide.empty()) return guidelinePoints;

    size_t numCurves = samplestoguide.size();        // 横向曲线数量
    size_t numSamples = samplestoguide[0].size();    // 每条曲线的采样点数

    guidelinePoints.resize(numSamples);

    // 对每个采样列 j，收集所有曲线的第 j 个点
    for (size_t j = 0; j < numSamples; ++j)
    {
        std::vector<ON_3dPoint> column;
        column.reserve(numCurves);

        for (size_t i = 0; i < numCurves; ++i)
        {
            // 保护性检查，防止数据长度不一致
            if (j < samplestoguide[i].size())
                column.push_back(samplestoguide[i][j]);
        }

        guidelinePoints[j] = column;
    }

    return guidelinePoints;
}

void SolveControlPoints_1Der(
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

    //输出基函数矩阵
    std::cout << "Basis function matrix N:" << std::endl;
    std::cout << N_mat << std::endl;

    Eigen::RowVectorXd dN_start(n + 1);
    Eigen::RowVectorXd dN_end(n + 1);

    // 参数 = 第一个
    double u_start = params.front();
    BasisResult br_start = computeBasisAndDerivatives_ON(degree, knotVector, u_start);
    for (int j = 0; j <= n; ++j) {
        dN_start(j) = br_start.dN[j];
    }

    // 参数 = 最后一个
    double u_end = params.back();
    BasisResult br_end = computeBasisAndDerivatives_ON(degree, knotVector, u_end);
    for (int j = 0; j <= n; ++j) {
        dN_end(j) = br_end.dN[j];
    }

    std::cout << "first derivative at start (u=" << u_start << "):\n" << dN_start << std::endl;
    std::cout << "first derivative at end   (u=" << u_end << "):\n" << dN_end << std::endl;
    // 新尺寸：多 2 行
    Eigen::MatrixXd N_new(N_mat.rows() + 2, N_mat.cols());

    // 第 1 行 = ddN_start
    N_new.row(0) = dN_start;

    // 中间 = 原 N_mat
    N_new.block(1, 0, N_mat.rows(), N_mat.cols()) = N_mat;

    // 最后一行 = ddN_end
    N_new.row(N_new.rows() - 1) = dN_end;


    std::cout << "Basis function matrix N_final:" << std::endl;
    std::cout << N_new << std::endl;
    //构造Y矩阵 (m行3列)
    Eigen::MatrixXd Y_mat(m + 2, 3);
    for (int i = 0; i < m; ++i) {
        Y_mat(i + 1, 0) = points[i].x;
        Y_mat(i + 1, 1) = points[i].y;
        Y_mat(i + 1, 2) = points[i].z;
    }
    ON_3dPoint Q0(0.0, 0.0, 0.0);

    Y_mat.row(0) = Eigen::RowVector3d(Q0.x, Q0.y, Q0.z);      // 第一行 = 第二行
    Y_mat.row(m + 1) = Eigen::RowVector3d(Q0.x, Q0.y, Q0.z);      // 最后一行 = 倒数第二行
    //输出基函数矩阵
    std::cout << "Basis function matrix Y:" << std::endl;
    std::cout << Y_mat << std::endl;
    // 只有j在[startIndex, startIndex+degree]区间内时基函数非零，赋对应值

    Eigen::MatrixXd Q_mat = N_new.colPivHouseholderQr().solve(Y_mat);

    // 转换结果到控制点数组
    controlPoints.resize(n + 1);
    for (int i = 0; i <= n; ++i) {
        controlPoints[i] = ON_3dPoint(Q_mat(i, 0), Q_mat(i, 1), Q_mat(i, 2));
    }
}




void SolveControlPoints_2Der(
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

    ////输出基函数矩阵
    //std::cout << "Basis function matrix N:" << std::endl;
    //std::cout << N_mat << std::endl;
    Eigen::RowVectorXd ddN_start(n + 1);
    Eigen::RowVectorXd ddN_end(n + 1);

    // 参数 = 第一个
    double u_start = params.front();
    BasisResult br_start = computeBasisAndDerivatives_ON(degree, knotVector, u_start);
    for (int j = 0; j <= n; ++j) {
        ddN_start(j) = br_start.ddN[j];
    }

    // 参数 = 最后一个
    double u_end = params.back();
    BasisResult br_end = computeBasisAndDerivatives_ON(degree, knotVector, u_end);
    for (int j = 0; j <= n; ++j) {
        ddN_end(j) = br_end.ddN[j];
    }

    //std::cout << "Second derivative at start (u="<< u_start <<"):\n" << ddN_start << std::endl;
    //std::cout << "Second derivative at end   (u="<< u_end <<"):\n" << ddN_end << std::endl;
    // 新尺寸：多 2 行
    Eigen::MatrixXd N_new(N_mat.rows() + 2, N_mat.cols());

    // 第 1 行 = ddN_start
    N_new.row(0) = ddN_start;

    // 中间 = 原 N_mat
    N_new.block(1, 0, N_mat.rows(), N_mat.cols()) = N_mat;

    // 最后一行 = ddN_end
    N_new.row(N_new.rows() - 1) = ddN_end;


    //std::cout << "Basis function matrix N_final:" << std::endl;
    //std::cout << N_new << std::endl;
    //构造Y矩阵 (m行3列)
    Eigen::MatrixXd Y_mat(m+2 , 3);
    for (int i = 0; i < m; ++i) {
        Y_mat(i+1 , 0) = points[i].x;
        Y_mat(i+1 , 1) = points[i].y;
        Y_mat(i+1 , 2) = points[i].z;
    }
    ON_3dPoint Q0(0.0, 0.0, 0.0);

    Y_mat.row(0) = Eigen::RowVector3d(Q0.x, Q0.y, Q0.z);      // 第一行 = 第二行
    Y_mat.row(m + 1) = Eigen::RowVector3d(Q0.x, Q0.y, Q0.z);      // 最后一行 = 倒数第二行
    ////输出基函数矩阵
    //std::cout << "Basis function matrix Y:" << std::endl;
    //std::cout << Y_mat << std::endl;
    // 只有j在[startIndex, startIndex+degree]区间内时基函数非零，赋对应值

    Eigen::MatrixXd Q_mat = N_new.colPivHouseholderQr().solve(Y_mat);

    // 转换结果到控制点数组
    controlPoints.resize(n + 1);
    for (int i = 0; i <= n; ++i) {
        controlPoints[i] = ON_3dPoint(Q_mat(i, 0), Q_mat(i , 1), Q_mat(i, 2));
    }

}




std::vector<ON_NurbsCurve> GenerateShapeGuides(
    std::vector<std::vector<ON_3dPoint>>& guidelineSamples,//引导线上的点
    int degree,
    int dim,
    int order,
    const std::string& debugObjName,
    std::vector<std::vector<double>>& allGuideParams)
{
    std::vector<ON_NurbsCurve> allguidecurves;
    for (size_t i = 0; i < guidelineSamples.size(); ++i)
    {
        auto params = chordLengthParam(guidelineSamples[i]);    // 弦长参数化
        // 保存下来（⭐ 关键）
        allGuideParams.push_back(params);
        //std::cout << "parameters: ";
        //for (double p : params) {
        //    std::cout << p << " ";
        //}
        //std::cout << std::endl;
        //////////////////////////有自由度，弦长参数直接做节点//////////////
        //得到节点向量
        std::vector<double> guideknots(params.size() + 2 * (degree - 1));

        // 前端 clamped
        for (int q = 0; q < degree - 1; q++)
            guideknots[q] = 0.0;
        // 后端 clamped
        for (int r = degree - 1 + params.size(); r < params.size() + 2 * (degree - 1); r++)
            guideknots[r] = 1.0;

        // 中间 knots
        for (int j = degree - 1; j <= degree - 1 + params.size() - 1; j++) {
            guideknots[j] = params[j - degree + 1];
        }

        //std::cout << "guideknots: ";
        //for (double p : guideknots) {
        //    std::cout << p << " ";
        //}
        //std::cout << std::endl;
       
        std::vector<ON_3dPoint> guide_controlPoints; // 先分配外层大小
        // 求解控制点，controlPoints[i]会被填充
        SolveControlPoints_2Der(degree, guideknots, params, guidelineSamples[i], guide_controlPoints);
        /*for (size_t s = 0; s < guide_controlPoints.size(); ++s) {
            const ON_3dPoint& pt = guide_controlPoints[s];
            std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
        }*/
        //std::cout << std::endl;
        std::vector<double> guideweights(guide_controlPoints.size());
        for (size_t t = 0; t < guide_controlPoints.size(); ++t) {
            guideweights[t] = 1;
        }
        ON_NurbsCurve guidecurves = CreateNURBSCurve(dim, false, order, guide_controlPoints, guideweights, guideknots);
        allguidecurves.push_back(guidecurves);
    
    }
    // 只是可视化引导线(可选)
    std::vector<std::vector<ON_3dPoint>> allguideSampledCurves;
    for (int i = 0; i < allguidecurves.size(); i++)
    {
        auto sampled = sampleCurve(allguidecurves[i], 1000);
        allguideSampledCurves.push_back(sampled);
    }
    // 写入到OBJ，启用横向连接线
    writeMultiCurvesWithCrossLinesToOBJ(allguideSampledCurves, debugObjName, false);
    return allguidecurves;
}

//// 生成 T 样条曲面采样点（二维顶点网格）
//std::vector<std::vector<ON_3dPoint>>
//SampleTSplineSurface(
//    int v_samples,//allguidecurves.size()
//    int u_samples,//shape_point_num
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& ALLREcurves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v,
//    double u_min ,
//    double u_max,
//    double v_min,
//    double v_max
//)
//{
//    std::vector<std::vector<ON_3dPoint>> vertices;
//    vertices.resize(v_samples);
//
//    double u_step = (u_max - u_min) / (u_samples - 1);
//    double v_step = (v_max - v_min) / (v_samples - 1);
//
//    for (int j = 0; j < v_samples; ++j) {
//
//        vertices[j].reserve(u_samples);
//
//        for (int i = 0; i < u_samples; ++i) {
//
//            double u = u_min + i * u_step;
//            double v = v_min + j * v_step;
//
//            // 调用你的 T 样条评价函数
//            ON_3dPoint p =
//                computeSurfacePoint(
//                    v, u,            // （注意你原代码里 uv 顺序反过来的）
//                    tmesh,
//                    ALLREcurves,
//                    v_knots,
//                    degree_u,
//                    degree_v
//                );
//
//            vertices[j].push_back(p);
//        }
//    }
//
//    return vertices;
//}

//std::vector<std::vector<ON_3dVector>> Distance_shape_surface(
//    const std::vector<std::vector<ON_3dPoint>>& A,
//    const std::vector<std::vector<ON_3dPoint>>& B)
//{
//    std::vector<std::vector<ON_3dVector>> result;
//    result.resize(A.size());
//
//    for (size_t i = 0; i < A.size(); ++i) {
//        result[i].resize(A[i].size());
//        for (size_t j = 0; j < A[i].size(); ++j) {
//            result[i][j] = A[i][j] - B[i][j];  // ON_3dPoint 支持减法
//        }
//    }
//
//    return result;
//}

void SavePointsToOBJ_pro(const std::vector<ON_3dPoint>& points, const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    for (const auto& pt : points) {
        ofs << "v " << pt.x << " " << pt.y << " " << pt.z << "\n";
    }
    ofs.close();
    std::cout << "Saved " << points.size() << " points to " << filename << std::endl;
}
std::vector<TRow> buildTMesh_pro(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt,
    const std::vector<ON_NurbsCurve>& allguidecurves,
    std::vector<std::vector<double>>& Adaptive_knots,
    int degree)
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
            /*double v_up = (2.0 * para_v_firstcpt[i] + para_v_firstcpt[i + 1]) / 3.0;
            double v_down = (para_v_firstcpt[i] + 2.0 * para_v_firstcpt[i + 1]) / 3.0;*/
            double v1 = para_v_firstcpt[i];
            double v2 = para_v_firstcpt[i + 1];

            std::vector<double> candidates_up;
            std::vector<double> candidates_down;

            // 1️⃣ 每条引导线单独算
            for (int g = 0; g < allguidecurves.size(); ++g)
            {
                double totalLength =
                    ComputeLocalArcLength(
                        allguidecurves[g],
                        v1,
                        v2
                    );

                double target1 = totalLength / 3.0;
                double target2 = 2.0 * totalLength / 3.0;

                double v_candidate_up =
                    FindParameterByArcLength(
                        allguidecurves[g],
                        v1,
                        v2,
                        target1
                    );

                double v_candidate_down =
                    FindParameterByArcLength(
                        allguidecurves[g],
                        v1,
                        v2,
                        target2
                    );

                candidates_up.push_back(v_candidate_up);
                candidates_down.push_back(v_candidate_down);
            }

            // 2️⃣ 排序
            std::sort(candidates_up.begin(), candidates_up.end());
            std::sort(candidates_down.begin(), candidates_down.end());

            // 3️⃣ 取中位数
            double v_up =
                candidates_up[candidates_up.size() / 2];

            double v_down =
                candidates_down[candidates_down.size() / 2];

            TRow row_up, row_down;
            row_up.v = v_up;
            row_up.u_positions = grevilleAbscissae(curves[i]);
            
            //1、找到每条引导线allguidecurves上对应参数位置v_up的点
            int Nu_up = curves[i].CVCount();
            row_up.control_points.resize(Nu_up);//控制点大小
            // 1. 在所有引导线上找 v_up 处的点
            std::vector<ON_3dPoint> sample_points_up(allguidecurves.size());//这里大小是引导线的大小
            for (int j = 0; j < allguidecurves.size(); ++j) {
                ON_3dPoint pt;
                allguidecurves[j].EvPoint(v_up, pt);
                sample_points_up[j] = pt;
            }
            /*       std::string filename_up = "sample_points_up_" + std::to_string(i) + ".obj";
          SavePointsToOBJ_pro(sample_points_up, filename_up);*/
          //2、找到对应的节点向量，反解控制点
            std::vector<double>knot_up = Adaptive_knots[i];
            //std::cout << "knot_up.size() = " << knot_up.size() << std::endl;
            //算采样点对应参数向量
            std::vector<double>para_up(allguidecurves.size());
            unsigned knotCount_up = curves[i].KnotCount();
            double u_start_up = knot_up[degree - 1];
            double u_end_up = knot_up[knotCount_up - degree];
            for (int j = 0; j < allguidecurves.size(); ++j) {
                para_up[j] = u_start_up + (u_end_up - u_start_up) * double(j) / (allguidecurves.size() - 1);
            }
            //std::cout << "para_up = [";
            //for (double val : para_up) {
            //    std::cout << val << ", ";
            //}
            //std::cout << "\b\b]" << std::endl; // \b\b 删除最后的逗号空格
            std::vector<double> knots_insert_up = knotsFromGreville(para_up, degree);
            //std::cout << "knots_insert_up = [";
            //for (double val : knots_insert_up) {
            //    std::cout << val << ", ";
            //}
            //std::cout << "\b\b]" << std::endl; // \b\b 删除最后的逗号空格
            std::vector<ON_3dPoint>tempora_cpts(knots_insert_up.size()-degree+1);
            SolveControlPoints_OpenNurbs(
                degree,                                       // 曲线次数-1，即degree=p
                knots_insert_up,           // 该NURBS的节点向量（非重复端点）16
                para_up,                // 参数点(u_0,u_1,...),每个对应一个空间点20
                sample_points_up,            // 空间点序列，对应参数点的曲线点坐标Y20
                tempora_cpts           // 输出的控制点序列Q，反求得
            );
            int dim = 3;
            // 4. 构建 NURBS 曲线，权重全部为1
            std::vector<double> weights(tempora_cpts.size(), 1.0);
            ON_NurbsCurve curves_insert_up = CreateNURBSCurve(dim, false, degree+1, tempora_cpts, weights, knots_insert_up);

            //int numSample_up = static_cast<int>(knot_up.size() - degree + 1); // 采样点数量
            //std::vector<ON_3dPoint> sampledPoints_up(numSample_up);
            //std::vector<double> sampledParams_up(numSample_up);  // 存储对应参数
            //double u_min = curves_insert_up.Knot(degree-1);                 // 第一个非重复节点
            //double u_max = curves_insert_up.Knot(curves_insert_up.KnotCount() - degree ); // 最后一个非重复节点
            //double step = (u_max - u_min) / (numSample_up - 1);

            //for (int i = 0; i < numSample_up; ++i) {
            //    double u = u_min + step * i;
            //    ON_3dPoint pt;
            //    curves_insert_up.EvPoint(u, pt);  // 采样曲线点
            //    sampledPoints_up[i] = pt;
            //    sampledParams_up[i] = u;
            //}
            double epsilon = 1e-3; // 误差容忍度
            auto sampledPtsup = SampleCurveBySpans(curves_insert_up, epsilon);
            std::vector<double> uppara = chordLengthParam(sampledPtsup);
      //std::string filename_up = "sample_points_up_" + std::to_string(i) + ".obj";
      //SavePointsToOBJ_pro(sampledPtsup, filename_up);

   

            SolveControlPoints_OpenNurbs(
                degree,                                       // 曲线次数-1，即degree=p
                knot_up,           // 该NURBS的节点向量（非重复端点）16
                uppara,                // 参数点(u_0,u_1,...),每个对应一个空间点20
                sampledPtsup,            // 空间点序列，对应参数点的曲线点坐标Y20
                row_up.control_points           // 输出的控制点序列Q，反求得
            );


            ///////////////////////////////////下面那条//////////////////////////
            row_down.v = v_down;
            row_down.u_positions = grevilleAbscissae(curves[i + 1]);
            //找到每条引导线上对应参数位置v_up的点
            //1、找到每条引导线allguidecurves上对应参数位置v_up的点
            int Nu_down = curves[i+1].CVCount();
            row_down.control_points.resize(Nu_down);//控制点大小
            // 1. 在所有引导线上找 v_up 处的点
            std::vector<ON_3dPoint> sample_points_down(allguidecurves.size());//这里大小是引导线的大小
            for (int j = 0; j < allguidecurves.size(); ++j) {
                ON_3dPoint pt;
                allguidecurves[j].EvPoint(v_down, pt);
                sample_points_down[j] = pt;
            }
            //// 生成 sample_points_down 后
            //std::string filename_down = "sample_points_down_" + std::to_string(i) + ".obj";
            //SavePointsToOBJ_pro(sample_points_down, filename_down);
            //2、找到对应的节点向量，反解控制点
            std::vector<double>knot_down = Adaptive_knots[i+1];
            //算采样点对应参数向量
            std::vector<double>para_down(allguidecurves.size());
            unsigned knotCount_down = curves[i+1].KnotCount();
            double u_start_down = knot_down[degree - 1];
            double u_end_down = knot_down[knotCount_down - degree];
            for (int j = 0; j < allguidecurves.size(); ++j) {
                para_down[j] = u_start_down + (u_end_down - u_start_down) * double(j) / (allguidecurves.size() - 1);
            }









            std::vector<double> knots_insert_down = knotsFromGreville(para_down, degree);
            //std::cout << "knots_insert_down = [";
            //for (double val : knots_insert_down) {
            //    std::cout << val << ", ";
            //}
            //std::cout << "\b\b]" << std::endl; // \b\b 删除最后的逗号空格
            std::vector<ON_3dPoint>tempora_cpts_down(knots_insert_down.size() - degree + 1);
            SolveControlPoints_OpenNurbs(
                degree,                                       // 曲线次数-1，即degree=p
                knots_insert_down,           // 该NURBS的节点向量（非重复端点）16
                para_down,                // 参数点(u_0,u_1,...),每个对应一个空间点20
                sample_points_down,            // 空间点序列，对应参数点的曲线点坐标Y20
                tempora_cpts_down           // 输出的控制点序列Q，反求得
            );
            
            // 4. 构建 NURBS 曲线，权重全部为1
            std::vector<double> weights_down(tempora_cpts_down.size(), 1.0);
            ON_NurbsCurve curves_insert_down = CreateNURBSCurve(dim, false, degree + 1, tempora_cpts_down, weights_down, knots_insert_down);

            //int numSample_down = static_cast<int>(knot_down.size() - degree + 1); // 采样点数量
            //std::vector<ON_3dPoint> sampledPoints_down(numSample_down);
            //std::vector<double> sampledParams_down(numSample_down);  // 存储对应参数
            //double u_min_down = curves_insert_down.Knot(degree - 1);                 // 第一个非重复节点
            //double u_max_down = curves_insert_down.Knot(curves_insert_down.KnotCount() - degree); // 最后一个非重复节点
            //double step_down = (u_max_down - u_min_down) / (numSample_down - 1);

            //for (int i = 0; i < numSample_down; ++i) {
            //    double u = u_min_down + step_down * i;
            //    ON_3dPoint pt;
            //    curves_insert_down.EvPoint(u, pt);  // 采样曲线点
            //    sampledPoints_down[i] = pt;
            //    sampledParams_down[i] = u;
            //}

            auto sampledPtsdown = SampleCurveBySpans(curves_insert_down, epsilon);
            std::vector<double> downpara = chordLengthParam(sampledPtsdown);







//            std::string filename_down = "sample_points_down_" + std::to_string(i) + ".obj";
//SavePointsToOBJ_pro(sampledPtsdown, filename_down);





         
            //// 输出 knot_down 信息
            //std::cout << "knot_down.size() = " << knot_down.size() << ", values = [";
            //for (double val : knot_down) std::cout << val << ", ";
            //std::cout << "\b\b]" << std::endl;


            //// 输出 sampledParams_down 信息
            //std::cout << "sampledPtsdown.size() = " << sampledPtsdown.size()  << std::endl;

            //// 输出 sampledPoints_down 信息
            //std::cout << " downpara.size() = " << downpara.size() << std::endl;
            //

            //// 输出输出数组 row_down.control_points 尺寸
            //std::cout << "row_down.control_points.size() = " << row_down.control_points.size() << std::endl;

            SolveControlPoints_OpenNurbs(
                degree,                                       // 曲线次数-1，即degree=p
                knot_down,           // 该NURBS的节点向量（非重复端点）
                downpara,                // 参数点(u_0,u_1,...),每个对应一个空间点20
                sampledPtsdown,           // 空间点序列，对应参数点的曲线点坐标Y
                row_down.control_points           // 输出的控制点序列Q，反求得
            );
            //std::string filename_downcpt = "sample_points_down_cpt_" + std::to_string(i) + ".obj";
            //SavePointsToOBJ_pro(row_down.control_points, filename_downcpt);

            tmesh.push_back(row_up);
            tmesh.push_back(row_down);
        }
    }
    return tmesh;
}








