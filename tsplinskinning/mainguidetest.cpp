//#include "paramatch.h"
//#include "Ohmethod.h"
//#include "Penaltyfunc.h"
//#include "Optimization_fun.h"
//#include "CGTS.h"
//#include "pro_cgts.h"
//#include "meshcurvature.h"
//#include "readstp.h"
////#include "pro_cgts.h"
//
//void flattenCtrlPoints(
//    const std::vector<TRow>& tmesh,
//    Eigen::VectorXd& px,
//    Eigen::VectorXd& py,
//    Eigen::VectorXd& pz
//)
//{
//    int nCtrl = 0;
//    for (const auto& row : tmesh)
//        nCtrl += (int)row.control_points.size();
//
//    px.resize(nCtrl);
//    py.resize(nCtrl);
//    pz.resize(nCtrl);
//
//    int k = 0;
//    for (const auto& row : tmesh) {
//        for (const auto& P : row.control_points) {
//            px(k) = P.x;
//            py(k) = P.y;
//            pz(k) = P.z;
//            ++k;
//        }
//    }
//}
//void printSurfaceRowAtFixedV(
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& curves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v,
//    double u_min, double u_max,
//    double v_fixed,          // 固定的 v
//    int u_samples            // u 方向采样数
//) {
//    double u_step = (u_max - u_min) / (u_samples - 1);
//
//    std::cout << "u v x y z\n";
//
//    for (int i = 0; i < u_samples; ++i) {
//        double u = u_min + i * u_step;
//
//        ON_3dPoint p = computeSurfacePoint(
//            u, v_fixed,
//            tmesh, curves, v_knots,
//            degree_u, degree_v
//        );
//
//        std::cout
//            << u << " "
//            << v_fixed << " "
//            << p.x << " "
//            << p.y << " "
//            << p.z << std::endl;
//    }
//}
//
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
//std::vector<ON_3dPoint> getControlPoints(const ON_NurbsCurve& curve)
//{
//    std::vector<ON_3dPoint> ctrlPoints;
//    int cvCount = curve.CVCount(); // 控制点个数
//    for (int i = 0; i < cvCount; i++) {
//        ON_3dPoint pt;
//        curve.GetCV(i, pt);
//        ctrlPoints.push_back(pt);
//    }
//    return ctrlPoints;
//}
//void exportSurfaceRowAtFixedV_toOBJ(
//    const std::vector<TRow>& tmesh,
//    const std::vector<ON_NurbsCurve>& curves,
//    const std::vector<double>& v_knots,
//    int degree_u,
//    int degree_v,
//    double u_min, double u_max,
//    double v_fixed,
//    int u_samples,
//    const std::string& filename
//) {
//    std::vector<std::vector<ON_3dPoint>> curves_points;
//    std::vector<ON_3dPoint> row_points;
//
//    double u_step = (u_max - u_min) / (u_samples - 1);
//
//    for (int i = 0; i < u_samples; ++i) {
//        double u = u_min + i * u_step;
//
//        ON_3dPoint p = computeSurfacePoint(
//            u, v_fixed,
//            tmesh, curves, v_knots,
//            degree_u, degree_v
//        );
//
//        row_points.push_back(p);
//    }
//
//    curves_points.push_back(row_points);
//
//    writeMultiCurvesWithCrossLinesToOBJ(
//        curves_points,
//        filename,
//        false   // 只有一条曲线，不需要 cross lines
//    );
//
//    std::cout << "Surface row exported at v = "
//        << v_fixed << " -> " << filename << std::endl;
//}
//
//int main(int argc, char** argv) {
///////////////////////////////////////风扇2///////////////////////////////////////
//    int cv_count1 = 5;//控制点数量6或5
//    int dim = 3;
//    int order = 4;
//    //std::string filename = "POINTS.DAT"; // 你的文件路径
//    auto ctrl1 = ReadControlPointsFromFile("model5/POINTS1.txt");
//    std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1,1 };
//    std::vector<double> weights2 = { 1.0, 1.0, 1.0, 1.0,1 };
//    std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    
//    
//    //PrintNURBSCurveInfo(curve1);
//    auto ctrl2 = ReadControlPointsFromFile("model5/POINTS2.txt");
//    std::vector<double> knots2 = { 0.0, 0.0, 0.0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//    
//    
//    auto ctrl3 = ReadControlPointsFromFile("model5/POINTS3.txt");
//    std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//    
//    
//    auto ctrl4 = ReadControlPointsFromFile("model5/POINTS4.txt");
//    std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.2,0.6,1.0, 1.0, 1.0 };
//    // 创建曲线
//    ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//    
//    
//    std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4};
//    std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4};
//    std::vector<std::vector<ON_3dPoint>> allCpt;
//    for (const auto& c : allcurves) {
//        allCpt.push_back(getControlPoints(c));
//    }
//    writeMultiCurvesWithCrossLinesToOBJ(allCpt, "controlPoints.obj", false);
//
//    // 采样点数，可根据需求增减
//    int num_samples = 10;
//    // 构建两条或多条曲线的采样点
//    std::vector<std::vector<ON_3dPoint>> allSampledCurves;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        auto sampled = sampleCurve(allcurves[i], 1000);
//        allSampledCurves.push_back(sampled);
//    }
//    // 写入到OBJ，启用横向连接线
//    writeMultiCurvesWithCrossLinesToOBJ(allSampledCurves, "ug_curves.obj", false);
//    double epsilon = 1e-3; // 误差容忍度
//    std::vector<std::vector<ON_3dPoint>> AllReSampledCurves;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        std::string knot_filename = "knots" + std::to_string(i) + ".txt";
//        saveParametersToFile(Knots[i], knot_filename);
//        //重采样
//        auto sampledPts = SampleCurveBySpans(allcurves[i], epsilon);
//        //动态生成文件名，如 curve0.txt, curve1.txt
//        std::string filename = "resample_points" + std::to_string(i) + ".txt";
//        savePointToFile(sampledPts, filename);
//        AllReSampledCurves.push_back(sampledPts);
//    }
//
//
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
//    //int numCtrlPts = 15;
//    int degree = 3;
//    //auto Adaptive_knots = uniformKnotVector(numCtrlPts, degree);//注意，这里用的一条，说明之后每条曲线的控制点数量和节点都是一样的
//    std::vector<std::vector<double>> Adaptive_knots(allcurves.size());
//
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        std::string knot_filename = "Adaptive_knot" + std::to_string(i) + ".txt";
//
//        Adaptive_knots[i] = readFileSkipFirstLast(knot_filename);
//
//        std::cout << knot_filename << Adaptive_knots[i].size() << std::endl;
//    }
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
//        SolveControlPoints_OpenNurbs(degree, Adaptive_knots[i], Allparameters[i], AllReSampledCurves[i], controlPoints[i]);
//    }
//
//    //for (size_t i = 0; i < controlPoints.size(); ++i) {
//    //    std::cout << "Control points for column " << i << ":" << std::endl;
//    //    for (size_t j = 0; j < controlPoints[i].size(); ++j) {
//    //        const ON_3dPoint& pt = controlPoints[i][j];
//    //        std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//    //    }
//    //}
//    std::vector<double> weights(controlPoints[0].size());
//    for (size_t i = 0; i < controlPoints[0].size(); ++i) {
//        weights[i] = 1;
//    }
//
//    std::vector<ON_NurbsCurve> ALLREcurves(allcurves.size());
//    for (size_t i = 0; i < allcurves.size(); ++i) {
//        ALLREcurves[i] = CreateNURBSCurve(dim, false, order, controlPoints[i], weights, Adaptive_knots[i]);
//    }
//    ///////////////////设置参数一致化后的曲线信息////////////////////
//     // 可视化参数一致化后的曲线
//    std::vector<std::vector<ON_3dPoint>> allSampledCurves111;
//    for (int i = 0; i < ALLREcurves.size(); i++)
//    {
//        auto sampled = sampleCurve(ALLREcurves[i], 1000);
//        allSampledCurves111.push_back(sampled);
//    }
//    // 写入到OBJ，启用横向连接线
//    writeMultiCurvesWithCrossLinesToOBJ(allSampledCurves111, "ug_curves111.obj", false);
//
//
//    //////////////////////////////////////////用T样条蒙皮//////////////////////////////////////////
//             //先获得V方向参数
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
//    //std::cout << std::endl;
//    std::vector<TRow> tmesh = buildTMesh(allcurves, para_v_firstcpt);
//    /*std::vector<std::vector<ON_3dPoint>> cp0_rows;
//    for (const auto& row : tmesh) {
//        cp0_rows.push_back(row.control_points);
//    }*/
//
//    //writeMultiCurvesWithCrossLinesToOBJ(cp0_rows, "tmesh_rows0.obj", false);
//    //printTMesh(tmesh);
//    // 3) 从 tmesh 收集所有行的 v（包括插入行）
//    std::vector<double> greville_v = collectRowVs(tmesh);
//    //std::cout << "greville_v (from tmesh rows): ";
//    /*for (double vv : greville_v) std::cout << vv << " ";
//    std::cout << std::endl;*/
//
//    // 4) 由 greville_v（即 \tilde{u}_i）反推全局 v_knot（clamped）
//    int degree_v = 3; // 你的 v 方向次数
//    std::vector<double> v_knots = knotsFromGreville(greville_v, degree_v);//通过弦长参数法设立节点向量。这里没有用自适应节点，但是也可以用
//    // saveSurfaceToObjFile(tmesh, allcurves, v_knots, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface0.obj");
//     /*std::cout << "V-direction knot vector: ";
//     for (double kv : v_knots) std::cout << kv << " ";
//     std::cout << std::endl;*/
//
//     //// 打印矩阵
//     //testBasisMatrix(knotsll, 3, paramsll);
//    updateControlPointsWithInterpolation(
//        tmesh,
//        v_knots,  // V方向的公共节点向量
//        degree_v
//    );
//    //printTMesh(tmesh);
//    saveSurfaceToObjFile(tmesh, allcurves, v_knots, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 150, 150, "ug_tsurface.obj");
//    computeGlobalMeanCurvature(
//        100, 100, tmesh, allcurves, v_knots,
//        3, 3,
//        0.0, 1.0,
//        0.0, 1.0);
//    //////////////////////////////////////////////用对齐后的////////////////////////////////////////////////////////
//
//    //int degree_v = 3; //  v 方向次数
//    std::vector<ON_3dPoint>control_pointsfirst111;
//    for (size_t num = 0; num < ALLREcurves.size(); ++num) {
//        ON_3dPoint cv;
//        if (ALLREcurves[num].CVCount() > 0) {
//            ALLREcurves[num].GetCV(0, cv);
//            control_pointsfirst111.push_back(cv);
//        }
//    }
//    //for (size_t i = 0; i < control_pointsfirst111.size(); ++i) {
//    //    const auto& pt = control_pointsfirst111[i];
//    //    std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//    //}
//    std::vector<double> para_v_firstcpt111 = chordLengthParam(control_pointsfirst111); // 计算弦长参数化
//    /*std::cout << "para_v_firstcpt111 ";
//    for (double kv : para_v_firstcpt111) std::cout << kv << " ";
//    std::cout << std::endl;*/
//    std::vector<TRow> tmesh111 = buildTMesh(ALLREcurves, para_v_firstcpt111);
//
//    // 3) 从 tmesh 收集所有行的 v（包括插入行）
//    std::vector<double> greville_v111 = collectRowVs(tmesh111);
//    //std::cout << "greville_v (from tmesh rows): ";
//    //for (double vv : greville_v111) std::cout << vv << " ";
//    //std::cout << std::endl;
//
//    // 4) 由 greville_v（即 \tilde{u}_i）反推全局 v_knot（clamped）
//
//    std::vector<double> v_knots111 = knotsFromGreville(greville_v111, degree);//通过弦长参数法设立节点向量。
//
//    /*std::cout << "V-direction knot vector: ";
//    for (double kv : v_knots111) std::cout << kv << " ";
//    std::cout << std::endl;*/
//    //saveSurfaceToObjFile(tmesh111, ALLREcurves, v_knots111, order - 1, degree, 0.0, 1.0, 0.0, 1.0, 100, 100, "ug_tsurface1110.obj");
//    /*std::vector<std::vector<ON_3dPoint>> cp10_rows;
//    for (const auto& row : tmesh111) {
//        cp10_rows.push_back(row.control_points);
//    }
//
//    writeMultiCurvesWithCrossLinesToOBJ(cp10_rows, "tmesh_rows10.obj", true);*/
//    //// 打印矩阵
//    //testBasisMatrix(knotsll, 3, paramsll);
//    updateControlPointsWithInterpolation(
//        tmesh111,
//        v_knots111,  // V方向的公共节点向量
//        degree
//    );
//    saveSurfaceToObjFile(tmesh111, ALLREcurves, v_knots111, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 150, 150, "ug_tsurface111.obj");
//    computeGlobalMeanCurvature(
//        100, 100, tmesh111, ALLREcurves, v_knots111,
//        3, 3,
//        0.0, 1.0,
//        0.0, 1.0);
//    ///////////////////曲面光顺///////////////////////////////////////
////一、构造引导线
////1、在已经处理好的截面线上均匀采样
//    int guide_point_num = 10;//引导线数量
//    std::vector<std::vector<ON_3dPoint>> samplestoguide(ALLREcurves.size());
//    // 开始采样
//    for (size_t i = 0; i < ALLREcurves.size(); ++i)
//    {
//        auto pts = sampleCurve(
//            ALLREcurves[i],
//            guide_point_num);
//        /*if (pts.size() != guide_point_num)
//        {
//            std::cerr << "error:th" << i << " sample num is "
//                << pts.size() << "not equal with guide_point_num\n";
//        }*/
//        samplestoguide[i] = pts;
//    }
//    // 保存OBJ
//    //writeMultiCurvesWithCrossLinesToOBJ(samplestoguide, "shape_samples.obj", false);
//    // samplestoguide: 每条曲线的采样点
//    std::vector<std::vector<ON_3dPoint>> guidelineSamples = Transpose(samplestoguide);
//    //writeMultiCurvesWithCrossLinesToOBJ(guidelineSamples, "guidelines.obj", false);
//    //std::cout << "\n=== guidelineSamples 内容 ===\n";
//    //
//    //for (size_t i = 0; i < guidelineSamples.size(); ++i)
//    //{
//    //    std::cout << "引导线 " << i << "，点数量 = "
//    //        << guidelineSamples[i].size() << ":\n";
//    //
//    //    for (size_t j = 0; j < guidelineSamples[i].size(); ++j)
//    //    {
//    //        const auto& P = guidelineSamples[i][j];
//    //        std::cout << "  P" << j << " = ("
//    //            << P.x << ", " << P.y << ", " << P.z << ")\n";
//    //    }
//    //    std::cout << "\n";
//    //}
//
//    ////2、为每一条引导线设置节点向量，节点向量为引导线节点向量，节点向量的数量就是guide_point_num的数量
//    //2、求解引导线
//    std::vector<std::vector<double>> allGuideParams;
//    std::vector<ON_NurbsCurve> allguidecurves = GenerateShapeGuides(
//        guidelineSamples,
//        degree,
//        dim,
//        order,
//        "guidefitcurves_free_2der.obj", allGuideParams);
//    ////3、在引导线上均匀采点，得到形状点
//    //int shape_point_num = 50;//引导线上采样点数量
//    //std::vector<std::vector<ON_3dPoint>> shapepoints;
//    //for (size_t i = 0; i < allguidecurves.size(); ++i)
//    //{
//    //    auto pts = sampleCurve(allguidecurves[i], shape_point_num);
//    //    shapepoints.push_back(pts);
//    //}
//    ////输出引导线上点的坐标信息，用于和曲面点的信息匹配
//    ////for (size_t i = 0; i < shapepoints.size(); ++i) {
//    ////    std::cout << "Group " << i << ":\n";
//    ////    for (size_t j = 0; j < shapepoints[i].size(); ++j) {
//    ////        const ON_3dPoint& p = shapepoints[i][j];
//    ////        std::cout << "  (" << p.x << ", " << p.y << ", " << p.z << ")\n";
//    ////    }
//    ////}
//    //writeMultiCurvesWithCrossLinesToOBJ(shapepoints, "shapepoint.obj", false);
//    //
//    //std::vector<std::vector<ON_3dPoint>> shapepoints_T = Transpose(shapepoints);
//
//
//    std::vector<TRow> tmesh111_pro =
//        buildTMesh_pro_pro(ALLREcurves,
//            para_v_firstcpt111,
//            allguidecurves,
//            Adaptive_knots,
//            degree, allGuideParams);
//
//    // 3) 从 tmesh 收集所有行的 v（包括插入行）
//    std::vector<double> greville_v111_pro = collectRowVs(tmesh111_pro);
//    //std::cout << "greville_v (from tmesh rows): ";
//    //for (double vv : greville_v111) std::cout << vv << " ";
//    //std::cout << std::endl;
//
//    // 4) 由 greville_v（即 \tilde{u}_i）反推全局 v_knot（clamped）
//
//    std::vector<double> v_knots111_pro = knotsFromGreville(greville_v111_pro, degree);//通过弦长参数法设立节点向量。
//
//    /* std::cout << "V-direction_pro knot vector: ";
//     for (double kv : v_knots111_pro) std::cout << kv << " ";
//     std::cout << std::endl;*/
//
//     //// 打印矩阵
//     //testBasisMatrix(knotsll, 3, paramsll);
//    std::vector<std::vector<ON_3dPoint>> cp_rows1;
//    for (const auto& row : tmesh111_pro) {
//        cp_rows1.push_back(row.control_points);
//    }
//
//    writeMultiCurvesWithCrossLinesToOBJ(cp_rows1, "tmesh_rows1.obj", true);
//
//
//    // 1. 保存更新前的 TMesh（包含所有控制点）
//    //tmesh111_pro[2].v += 0.2;
//    redistributeVParameters(tmesh111_pro);
//    std::vector<double> greville_v111_pro_change = collectRowVs(tmesh111_pro);
//    std::vector<double> v_knots111_pro_change = knotsFromGreville(greville_v111_pro_change, degree);//通过弦长参数法设立节点向量。
//    //
//    //std::cout << "V-direction_pro_change knot vector: ";
//    //for (double kv : v_knots111_pro_change) std::cout << kv << " ";
//    //std::cout << std::endl;
//    updateControlPointsWithInterpolation(
//        tmesh111_pro,
//        v_knots111_pro_change,  // V方向的公共节点向量
//        degree
//    );
//    //std::vector<std::vector<ON_3dPoint>> cp0_rows111_pro;
//    //for (const auto& row : tmesh111_pro) {
//    //    cp0_rows111_pro.push_back(row.control_points);
//    //}
//
//    //writeMultiCurvesWithCrossLinesToOBJ(cp0_rows111_pro, "tmesh_row111_pro.obj", false);
//    //printTMesh(tmesh111_pro);
//    std::vector<std::vector<ON_3dPoint>> cp_rows2;
//    for (const auto& row : tmesh111_pro) {
//        cp_rows2.push_back(row.control_points);
//    }
//
//    writeMultiCurvesWithCrossLinesToOBJ(cp_rows2, "tmesh_rows2.obj", true);
//    saveSurfaceToObjFile(tmesh111_pro, ALLREcurves, v_knots111_pro_change, order - 1, degree, 0.0, 1.0, 0.0, 1.0, 150, 150, "ug_tsurface111_pro.obj");
//    computeGlobalMeanCurvature(
//        100, 100, tmesh111_pro, ALLREcurves, v_knots111_pro_change,
//        3, 3,
//        0.0, 1.0,
//        0.0, 1.0);
//
//    return 0;
//
//}