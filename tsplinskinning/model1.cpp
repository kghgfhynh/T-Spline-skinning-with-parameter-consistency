//
//
//
//#include "paramatch.h"
//#include "Ohmethod.h"
//#include "Penaltyfunc.h"
//// 读取控制点（支持逗号分隔）
//std::vector<ON_3dPoint> ReadControlPointsFromFile(const std::string& filename)
//{
//    std::vector<ON_3dPoint> points;
//    std::ifstream infile(filename);
//    if (!infile.is_open()) {
//        std::cerr << "无法打开文件: " << filename << std::endl;
//        return points;
//    }
//
//    std::string line;
//    while (std::getline(infile, line))
//    {
//        // 用 stringstream 解析一行
//        std::replace(line.begin(), line.end(), ',', ' '); // 把逗号替换为空格
//        std::stringstream ss(line);
//        double x, y, z;
//        if (ss >> x >> y >> z)
//            points.emplace_back(x, y, z);
//    }
//
//    infile.close();
//    return points;
//}
//
//
//int main(int argc, char** argv) {
//    //int cv_count1 = 7;
//    //int dim = 3;
//    //int order = 4;
//    ////std::string filename = "POINTS.DAT"; // 你的文件路径
//    //auto ctrl1 = ReadControlPointsFromFile("POINTS1.DAT");
//    //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1,1,1 };
//    //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("POINTS2.DAT");
//    //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//    //auto ctrl3 = ReadControlPointsFromFile("POINTS3.DAT");
//    //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//    //auto ctrl4 = ReadControlPointsFromFile("POINTS4.DAT");
//    //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.7, 0.7,0.9,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//    //auto ctrl5 = ReadControlPointsFromFile("POINTS5.DAT");
//    //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//    //auto ctrl6 = ReadControlPointsFromFile("POINTS6.DAT");
//    //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//    //auto ctrl7 = ReadControlPointsFromFile("POINTS7.DAT");
//    //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//
//    //auto ctrl8 = ReadControlPointsFromFile("POINTS8.DAT");
//    //std::vector<double> knots8 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights1, knots8);
//
//    //auto ctrl9 = ReadControlPointsFromFile("POINTS9.DAT");
//    //std::vector<double> knots9 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights1, knots9);
//
//    //auto ctrl10 = ReadControlPointsFromFile("POINTS10.DAT");
//    //std::vector<double> knots10 = { 0.0, 0.0, 0.0,0.43, 0.6,0.7,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights1, knots10);
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10};
//    int cv_count1 = 5;
//    int dim = 3;
//    int order = 4;
//    //std::string filename = "POINTS.DAT"; // 你的文件路径
//    auto ctrl1 = ReadControlPointsFromFile("UGfigs/model2/POINTS1.DAT");
//    std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 };
//    std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    //PrintNURBSCurveInfo(curve1);
//    auto ctrl2 = ReadControlPointsFromFile("UGfigs/model2/POINTS2.DAT");
//    std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//    auto ctrl3 = ReadControlPointsFromFile("UGfigs/model2/POINTS3.DAT");
//    std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//    auto ctrl4 = ReadControlPointsFromFile("UGfigs/model2/POINTS4.DAT");
//    std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//    auto ctrl5 = ReadControlPointsFromFile("UGfigs/model2/POINTS5.DAT");
//    std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//    auto ctrl6 = ReadControlPointsFromFile("UGfigs/model2/POINTS6.DAT");
//    std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//    auto ctrl7 = ReadControlPointsFromFile("UGfigs/model2/POINTS7.DAT");
//    std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//
//
//    std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7 };
//    // 采样点数，可根据需求增减
//    int num_samples = 10;
//    // 构建两条或多条曲线的采样点
//    std::vector<std::vector<ON_3dPoint>> allSampledCurves;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        auto sampled = sampleCurve(allcurves[i], num_samples);
//        allSampledCurves.push_back(sampled);
//    }
//    // 写入到OBJ，启用横向连接线
//    writeMultiCurvesWithCrossLinesToOBJ(allSampledCurves, "ug_curves.obj", false);
//    double epsilon = 1e-3; // 误差容忍度
//    std::vector<std::vector<ON_3dPoint>> AllReSampledCurves;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        //重采样
//        auto sampledPts = SampleCurveBySpans(allcurves[i], epsilon);
//        //动态生成文件名，如 curve0.txt, curve1.txt
//        std::string filename = "resample_points" + std::to_string(i) + ".txt";
//        savePointToFile(sampledPts, filename);
//        AllReSampledCurves.push_back(sampledPts);
//    }
//    //重采样的点作为输入点输入到matlab
//
//    ////////////////////重采样之后进行拟合////////////////////////////
//    //接下来，对每条曲线上采的点进行弦长参数化
//    std::vector<std::vector<double>> Allparameters;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        std::vector<double> parameters = chordLengthParam(AllReSampledCurves[i]);
//        // 动态生成文件名，如 "para0", "para1", ...
//        std::string filename = "para" + std::to_string(i) + ".txt";
//        saveParametersToFile(parameters, filename);
//        Allparameters.push_back(parameters);
//
//    }
//
//    //假设节点向量均匀的
//    int numCtrlPts = 15;
//    int degree = 3;
//    auto Adaptive_knots = uniformKnotVector(numCtrlPts, degree);//注意，这里用的一条，说明之后每条曲线的控制点数量和节点都是一样的
//    //std::vector<std::vector<double>> Adaptive_knots;
//    //
//    //for (int i=0;i<allcurves.size();i++)
//    //{
//    //    std::string knot_filename = "Adaptive_knot" + std::to_string(i) + ".txt";
//    //    Adaptive_knots[i] = readFileSkipFirstLast(knot_filename);
//    //}
//    ////输出节点向量
//    //for (size_t i = 0; i < Adaptive_knots.size(); ++i)
//    //{
//    //    std::cout << "Adaptive_knots[" << i << "]: ";
//    //    for (size_t j = 0; j < Adaptive_knots[i].size(); ++j)
//    //    {
//    //        std::cout << Adaptive_knots[i][j];
//    //        if (j < Adaptive_knots[i].size() - 1) std::cout << ", ";
//    //    }
//    //    std::cout << std::endl;
//    //}
//    //////////////////设置参数一致化后的曲线信息//////////////
//    //得到参数点以及节点向量后，反解控制点，这个控制点是参数处理后的曲线
//    std::vector<std::vector<ON_3dPoint>> controlPoints(allcurves.size()); // 先分配外层大小
//    for (size_t i = 0; i < allcurves.size(); ++i) {
//        //调用前清空或重置当前控制点容器
//        controlPoints[i].clear();
//
//        // 求解控制点，controlPoints[i]会被填充
//        SolveControlPoints_OpenNurbs(degree, Adaptive_knots, Allparameters[i], AllReSampledCurves[i], controlPoints[i]);
//    }
//
//    /*for (size_t i = 0; i < controlPoints.size(); ++i) {
//        std::cout << "Control points for column " << i << ":" << std::endl;
//        for (size_t j = 0; j < controlPoints[i].size(); ++j) {
//            const ON_3dPoint& pt = controlPoints[i][j];
//            std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//        }
//    }*/
//    std::vector<double> weights(controlPoints[0].size());
//    for (size_t i = 0; i < controlPoints[0].size(); ++i) {
//        weights[i] = 1;
//    }
//
//    std::vector<ON_NurbsCurve> ALLREcurves(allcurves.size());
//    for (size_t i = 0; i < allcurves.size(); ++i) {
//        ALLREcurves[i] = CreateNURBSCurve(dim, false, order, controlPoints[i], weights, Adaptive_knots);;
//    }
//    ///////////////////设置参数一致化后的曲线信息////////////////////
//     // 可视化参数一致化后的曲线
//    std::vector<std::vector<ON_3dPoint>> allSampledCurves111;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        auto sampled = sampleCurve(ALLREcurves[i], num_samples);
//        allSampledCurves111.push_back(sampled);
//    }
//    // 写入到OBJ，启用横向连接线
//    writeMultiCurvesWithCrossLinesToOBJ(allSampledCurves111, "ug_curves111.obj", true);
//
//
//    //////////////////////////////////////////用T样条蒙皮//////////////////////////////////////////
//        //先获得V方向参数
//    //获得第一个控制点
//    std::vector<ON_3dPoint>control_pointsfirst;
//    for (size_t num = 0; num < allcurves.size(); ++num) {
//        ON_3dPoint cv;
//        if (allcurves[num].CVCount() > 0) {
//            allcurves[num].GetCV(0, cv);
//            control_pointsfirst.push_back(cv);
//        }
//    }
//    /*for (size_t i = 0; i < control_pointsfirst.size(); ++i) {
//        const auto& pt = control_pointsfirst[i];
//        std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//    }*/
//    //这里计算弦长应该用曲线上的点，但是由于样条的特性，第一个控制点就在样条上
//    std::vector<double> para_v_firstcpt = chordLengthParam(control_pointsfirst); // 计算弦长参数化，这里是v方向的
//    /*std::cout << "parameters  v: ";
//    for (double p : para_v_firstcpt) {
//        std::cout << p << " ";
//    }*/
//    std::cout << std::endl;
//    std::vector<TRow> tmesh = buildTMesh(allcurves, para_v_firstcpt);
//    printTMesh(tmesh);
//    // 3) 从 tmesh 收集所有行的 v（包括插入行）
//    std::vector<double> greville_v = collectRowVs(tmesh);
//    //std::cout << "greville_v (from tmesh rows): ";
//    /*for (double vv : greville_v) std::cout << vv << " ";
//    std::cout << std::endl;*/
//
//    // 4) 由 greville_v（即 \tilde{u}_i）反推全局 v_knot（clamped）
//    int degree_v = 3; // 你的 v 方向次数
//    std::vector<double> v_knots = knotsFromGreville(greville_v, degree_v);//通过弦长参数法设立节点向量。这里没有用自适应节点，但是也可以用
//
//    /*std::cout << "V-direction knot vector: ";
//    for (double kv : v_knots) std::cout << kv << " ";
//    std::cout << std::endl;*/
//
//    //// 打印矩阵
//    //testBasisMatrix(knotsll, 3, paramsll);
//    updateControlPointsWithInterpolation(
//        tmesh,
//        v_knots,  // V方向的公共节点向量
//        degree_v
//    );
//    //printTMesh(tmesh);
//    saveSurfaceToObjFile(tmesh, allcurves, v_knots, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface.obj");
//
//    //////////////////////////////////////////////用对齐后的////////////////////////////////////////////////////////
//    std::vector<ON_3dPoint>control_pointsfirst111;
//    for (size_t num = 0; num < ALLREcurves.size(); ++num) {
//        ON_3dPoint cv;
//        if (ALLREcurves[num].CVCount() > 0) {
//            ALLREcurves[num].GetCV(0, cv);
//            control_pointsfirst111.push_back(cv);
//        }
//    }
//    /*for (size_t i = 0; i < control_pointsfirst111.size(); ++i) {
//        const auto& pt = control_pointsfirst111[i];
//        std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//    }*/
//    std::vector<double> para_v_firstcpt111 = chordLengthParam(control_pointsfirst111); // 计算弦长参数化
//    /*std::cout << "parameters  v: ";
//
//    std::cout << std::endl;*/
//    std::vector<TRow> tmesh111 = buildTMesh(ALLREcurves, para_v_firstcpt111);
//    printTMesh(tmesh111);
//    // 3) 从 tmesh 收集所有行的 v（包括插入行）
//    std::vector<double> greville_v111 = collectRowVs(tmesh111);
//    //std::cout << "greville_v (from tmesh rows): ";
//    //for (double vv : greville_v111) std::cout << vv << " ";
//    //std::cout << std::endl;
//
//    // 4) 由 greville_v（即 \tilde{u}_i）反推全局 v_knot（clamped）
//
//    std::vector<double> v_knots111 = knotsFromGreville(greville_v111, degree_v);//通过弦长参数法设立节点向量。
//
//    /*std::cout << "V-direction knot vector: ";
//    for (double kv : v_knots111) std::cout << kv << " ";
//    std::cout << std::endl;*/
//
//    //// 打印矩阵
//    //testBasisMatrix(knotsll, 3, paramsll);
//    updateControlPointsWithInterpolation(
//        tmesh111,
//        v_knots111,  // V方向的公共节点向量
//        degree_v
//    );
//    //printTMesh(tmesh111);
//    saveSurfaceToObjFile(tmesh111, ALLREcurves, v_knots111, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface111.obj");
//
//
//    return 0;
//
//}
//















///////////////////////////////未修正代码LSPIA///////////////////////////////////////////
// double sumDeltaSquared(const std::vector<std::vector<ON_3dVector>>& d)
//{
//    double v = 0.0;
//    for (auto& row : d)
//        for (auto& dv : row)
//            v += dv.LengthSquared();
//    return v;
//}
//void computeDeltaForAllRows(
//    std::vector<std::vector<ON_3dVector>>& delta_num,
//    std::vector<std::vector<double>>& delta_den,
//    const std::vector<std::vector<ON_3dVector>>& delta_l,
//    const std::vector<ON_NurbsCurve>& ALLREcurves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v
//) {
//    int Nv = delta_num.size();
//
//
//    // 清零
//    for (int i = 0; i < Nv; ++i)
//    {
//        std::fill(delta_den[i].begin(), delta_den[i].end(), 0.0);
//        for (auto& v : delta_num[i]) v = ON_3dVector(0, 0, 0);
//    }
//
//    int shape_point_num = delta_l.size();
//    int guide_point_num = delta_l[0].size();
//
//    // 参数
//    std::vector<double> u_params(guide_point_num);
//    std::vector<double> v_params(shape_point_num);
//
//    for (int i = 0; i < guide_point_num; ++i)
//        u_params[i] = double(i) / (guide_point_num - 1);
//
//    for (int j = 0; j < shape_point_num; ++j)
//        v_params[j] = double(j) / (shape_point_num - 1);
//
//    // 遍历所有采样点
//    for (int iv = 0; iv < shape_point_num; ++iv)
//    {
//        double v = v_params[iv];
//        std::vector<double> Mv_vals = computeBasisFunctionsAt(degree_v, v_knots, v);
//        for (int iu = 0; iu < guide_point_num; ++iu)
//        {
//            double u = u_params[iu];
//            ON_3dVector diff = delta_l[iv][iu];
//
//            for (int i = 0; i < Nv; ++i)
//            {
//                const auto& curve = ALLREcurves[i];
//                std::vector<double> u_knots(curve.KnotCount());
//                for (int kk = 0; kk < curve.KnotCount(); ++kk)
//                    u_knots[kk] = curve.Knot(kk);
//
//                std::vector<double> Nu_vals = computeBasisFunctionsAt(degree_u, u_knots, u);
//
//                for (int j = 0; j < Nu_vals.size(); ++j)
//                {
//                    double Bij = Nu_vals[j] * Mv_vals[i];
//                    if (Bij == 0) continue;
//
//                    delta_num[i][j] += Bij * diff;
//                    delta_den[i][j] += Bij;
//                }
//            }
//        }
//    }
//}
//void updateControlPoints(
//    std::vector<std::vector<ON_3dPoint>>& cps,
//    const std::vector<std::vector<ON_3dVector>>& delta_num,
//    const std::vector<std::vector<double>>& delta_den
//) {
//    for (int i = 0; i < cps.size(); ++i)
//        for (int j = 0; j < cps[i].size(); ++j)
//            if (delta_den[i][j] > 1e-12)
//                cps[i][j] = cps[i][j] + delta_num[i][j] / delta_den[i][j];
//}
//
//#include <iostream>
//// 假设已包含 ON_3dVector 头文件，例如：
//// #include "opennurbs/opennurbs_3dvector.h"
//
//void printQuotients(
//    const std::vector<std::vector<ON_3dVector>>& delta_num,
//    const std::vector<std::vector<double>>& delta_den)
//{
//    const double eps = 1e-12;
//    for (size_t i = 0; i < delta_num.size(); ++i) {
//        const auto& row_num = delta_num[i];
//        const auto& row_den = delta_den[i];
//        for (size_t j = 0; j < row_num.size(); ++j) {
//            double d = row_den[j];
//            if (std::fabs(d) < eps) continue; // 跳过接近零的分母
//            ON_3dVector q = row_num[j] / d;
//            std::cout << "[" << i << "][" << j << "] = {"
//                << q.x << ", " << q.y << ", " << q.z << "}\n";
//        }
//    }
//}
//int max_outer = 5;      // τ, 交替次数
//int max_inner = 30;      // τ_s，LSPIA 最大迭代次数
//double eps0 = 1e-4;      // LSPIA 的收敛阈值
//// ================== 1. 初始化 delta_num / delta_den =====================
//int Nv = controlPoints.size();
//
//std::vector<std::vector<ON_3dVector>> delta_num(Nv);
//std::vector<std::vector<double>>      delta_den(Nv);
//
//for (int i = 0; i < Nv; ++i)
//{
//    int Nu_i = controlPoints[i].size();     // ? 每一行独立的控制点数量
//
//    delta_num[i].assign(Nu_i, ON_3dVector(0, 0, 0));
//    delta_den[i].assign(Nu_i, 0.0);
//}
//// 保持一个“当前用于评价/计算基函数”的行曲线集合
//// 初始它等于 ALLREcurves（由最初插值得到）
//std::vector<ON_NurbsCurve> ALLREcurves_current = ALLREcurves;
//std::vector<TRow> tmesh_current = tmesh111;
//std::vector<double> v_knots_current = v_knots111;
//// ================== 2. 外层 s 循环 =====================
//for (int s = 0; s < max_outer; ++s)
//{
//    std::cout << "\n===== OUTER ITERATION s = " << s << " =====\n";
//
//    // ---------------- LSPIA 内层 t 循环 -----------------
//    std::vector<std::vector<ON_3dVector>> delta_t;
//    std::vector<std::vector<ON_3dVector>> delta_tp1;
//
//    for (int t = 0; t < max_inner; ++t)
//    {
//        std::cout << "  -- LSPIA inner iteration t = " << t << std::endl;
//
//        // (1) 采样当前曲面 S^(s,t) —— 使用 current 集合
//        auto vertices = SampleTSplineSurface(
//            static_cast<int>(
//            allguidecurves.size()),   // v_samples = 当前行数
//            shape_point_num,
//            tmesh_current,
//            ALLREcurves_current,
//            v_knots_current,
//            degree, degree,
//            0.0, 1.0, 0.0, 1.0
//        );
//        auto vertices_T = Transpose(vertices);
//
//        // (2) 计算 δ^(s,t) = Q - S^(s,t)
//        delta_t = Distance_shape_surface(shapepoints_T, vertices_T);
//
//        // (3) 计算 Δ_i^(s,t)
//        computeDeltaForAllRows(
//            delta_num,
//            delta_den,
//            delta_t,
//            ALLREcurves_current,    // ? 当前曲线集合
//            v_knots_current,        // ? 当前 v 节点向量
//            degree,
//            degree
//        );
//
//
//        // (4) 更新控制点： P^(s,t+1) = P^(s,t) + Δ
//        updateControlPoints(controlPoints, delta_num, delta_den);
//        //printQuotients(delta_num,delta_den);
//        // IMPORTANT:
//        // controlPoints 发生改变后，必须立即重建一组对应的行曲线集合（ALLREcurves_new）
//        int Nv_now = static_cast<int>(controlPoints.size());
//        std::vector<ON_NurbsCurve> ALLREcurves_new(Nv_now);
//        for (int i = 0; i < Nv_now; ++i) {
//            // 注意：你用的是 Adaptive_knots[i]（每行自己的 knot）
//            ALLREcurves_new[i] = CreateNURBSCurve(
//                3,
//                false,
//                order,
//                controlPoints[i],
//                weights,
//                Adaptive_knots[i]
//            );
//        }
//        // (6) 用 ALLREcurves_new 构建临时 tmesh/v_knots 用于计算 δ^(t+1)
//        std::vector<ON_3dPoint> firstCpts;
//        firstCpts.reserve(ALLREcurves_new.size());
//        for (auto& c : ALLREcurves_new) {
//            ON_3dPoint cv;
//            if (c.CVCount() > 0) { c.GetCV(0, cv); firstCpts.push_back(cv); }
//            else firstCpts.push_back(ON_3dPoint(0, 0, 0)); // 防御性
//        }
//        auto change_para_v = chordLengthParam(firstCpts);
//        auto change_tmesh = buildTMesh(ALLREcurves_new, change_para_v);
//        auto change_greville = collectRowVs(change_tmesh);
//        auto change_v_knots = knotsFromGreville(change_greville, degree);
//        // (7) 采样 S^(s,t+1)（使用临时新集合）
//        auto vertices2 = SampleTSplineSurface(
//            static_cast<int>(allguidecurves.size()),
//            shape_point_num,
//            change_tmesh,
//            ALLREcurves_new,
//            change_v_knots,
//            degree, degree,
//            0.0, 1.0, 0.0, 1.0
//        );
//        auto vertices2_T = Transpose(vertices2);
//        auto delta_tp1 = Distance_shape_surface(shapepoints_T, vertices2_T);
//
//        // (8) 误差评估（带除零保护）
//        double err_t = sumDeltaSquared(delta_t);
//        double err_tp1 = sumDeltaSquared(delta_tp1);
//        double ratio = fabs(err_t < 1e-16 ? (err_tp1 < 1e-16 ? 0.0 : 1.0) : (err_tp1 / err_t - 1.0));
//        std::cout << "     error ratio = " << ratio << std::endl;
//
//        tmesh_current = change_tmesh;
//        v_knots_current = change_v_knots;
//        ALLREcurves_current = ALLREcurves_new;
//
//    }
//
//    //// 用当前 controlPoints 重建 ALLREcurves_for_interpolation
//    //int Nv_final = (int)controlPoints.size();
//    //std::vector<ON_NurbsCurve> ALLREcurves_for_interp(Nv_final);
//    //for (int i = 0; i < Nv_final; ++i) {
//    //    ALLREcurves_for_interp[i] = CreateNURBSCurve(
//    //        3, false, order,
//    //        controlPoints[i],
//    //        weights,
//    //        Adaptive_knots[i]
//    //    );
//
//    //}
//    //std::vector<ON_3dPoint>new_pointsfirst111_final;
//    //for (size_t num = 0; num < ALLREcurves_for_interp.size(); ++num) {
//    //    ON_3dPoint cv;
//    //    if (ALLREcurves_for_interp[num].CVCount() > 0) {
//    //        ALLREcurves_for_interp[num].GetCV(0, cv);
//    //        new_pointsfirst111_final.push_back(cv);
//    //    }
//    //}
//    //std::vector<double> new_para_v_firstcpt111 = chordLengthParam(new_pointsfirst111_final); // 计算弦长参数化
//    //// 重建全局 tmesh，greville，v_knots（基于更新后的行曲线）
//    //std::vector<TRow> new_mesh = buildTMesh(ALLREcurves_for_interp, new_para_v_firstcpt111); // para_v_firstcpt111 可依据情况更新
//    //auto new_vs = collectRowVs(new_mesh);
//    //auto new_v_knots = knotsFromGreville(new_vs, degree);
//
//    //// 应用论文中的插值公式（2.10,2.12）把 controlPoints 调回满足插值条件
//    //updateControlPointsWithInterpolation(new_mesh, new_v_knots, degree);
//
//    updateControlPointsWithInterpolation(tmesh_current, v_knots_current, degree);
//    ////// 更新用于下一轮评价的 tmesh111 / ALLREcurves / v_knots111
//    //tmesh111 = tmesh_current;
//    //v_knots111 = v_knots_current;
//    //ALLREcurves = ALLREcurves_current; // 将外层曲线集合替换为用 controlPoints 构建的版本
//    //这里也要算一下
//
//}
//std::cout << "\n===================== 交替迭代结束 =====================\n";
//
//// ============================================================
//// 1. 使用最终 controlPoints 重建所有行的 NURBS 曲线 ALLREcurves_final
//// ============================================================
//
//
////std::vector<ON_NurbsCurve> ALLREcurves_final(Nv);
////
////for (int i = 0; i < Nv; ++i)
////{
////    ALLREcurves_final[i] = CreateNURBSCurve(
////        3,                  // 维度
////        false,              // 非有理
////        order,              // u方向阶
////        controlPoints[i],   // 第 i 行控制点
////        weights,            // 可为 vector<double>() 或空
////       Adaptive_knots[i]         // 使用最终统一 v 节点向量（论文建议统一）
////    );
////}
////
////std::cout << "已根据最终 controlPoints 重建 ALLREcurves_final。" << std::endl;
////
////
////// ============================================================
////// 2. 重建最终 TMesh（使用最终控制点）
//////    注意：如果你在插值步骤中已更新 tmesh111 / v_knots111
//////          那么它们已经是最终的 TMesh 和节点向量
////// ============================================================
////std::vector<ON_3dPoint>control_pointsfirst111_final;
////for (size_t num = 0; num < ALLREcurves_final.size(); ++num) {
////    ON_3dPoint cv;
////    if (ALLREcurves_final[num].CVCount() > 0) {
////        ALLREcurves_final[num].GetCV(0, cv);
////        control_pointsfirst111_final.push_back(cv);
////    }
////}
////std::vector<double> para_v_firstcpt111_final = chordLengthParam(control_pointsfirst111_final); // 计算弦长参数化
////std::vector<TRow> tmesh111_final = buildTMesh(ALLREcurves_final, para_v_firstcpt111_final);
////std::vector<double> greville_v111_final = collectRowVs(tmesh111_final);
////std::vector<double> v_knots111_final = knotsFromGreville(greville_v111_final, degree);//通过弦长参数法设立节点向量。
////
//
//
//
//// ============================================================
//// 4. 输出 OBJ 曲面（最终结果）
//// ============================================================
//
////saveSurfaceToObjFile(
////    tmesh111_final,
////    ALLREcurves_final,
////    v_knots111_final,
////    degree,                // u方向次数
////    degree,              // v方向次数
////    0.0, 1.0,
////    0.0, 1.0,
////    100, 100,              // 网格细分
////    "final_tsurface.obj"
////);
////
////std::cout << "已输出最终曲面 OBJ: final_tsurface.obj\n" << std::endl;
//
//saveSurfaceToObjFile(
//    tmesh_current,
//    ALLREcurves_current,
//    v_knots_current,
//        degree,                // u方向次数
//        degree,              // v方向次数
//        0.0, 1.0,
//        0.0, 1.0,
//        100, 100,              // 网格细分
//        "final_tsurface.obj"
//    );
//
//    std::cout << "已输出最终曲面 OBJ: final_tsurface.obj\n" << std::endl;
//
////// ============================================================
////// 5. 你可选地输出控制点网格，便于调试
////// ============================================================
////std::cout << "最终控制点：" << std::endl;
////for (int i = 0; i < Nv; ++i)
////{
////    std::cout << "Row " << i << ": ";
////    for (int j = 0; j < Nu; ++j)
////    {
////        auto& P = controlPoints[i][j];
////        std::cout << "(" << P.x << "," << P.y << "," << P.z << ")  ";
////    }
////    std::cout << "\n";
////}
////
////std::cout << "===================== 最终输出完成 =====================\n";
//
//