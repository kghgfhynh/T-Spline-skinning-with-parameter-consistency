//#include "paramatch.h"
//#include "Ohmethod.h"
//#include "Penaltyfunc.h"
//#include "Optimization_fun.h"
//#include "CGTS.h"
//#include "pro_cgts.h"
//#include "meshcurvature.h"
//#include "readfile.h"
//#include "InterpolationError.h"
//#include <iomanip>
////#include "readstp.h"
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
////// 读取控制点（支持逗号分隔）
////std::vector<ON_3dPoint> ReadControlPointsFromFile(const std::string& filename)
////{
////    std::vector<ON_3dPoint> points;
////    std::ifstream infile(filename);
////    if (!infile.is_open()) {
////        std::cerr << "无法打开文件: " << filename << std::endl;
////        return points;
////    }
////
////    std::string line;
////    while (std::getline(infile, line))
////    {
////        // 用 stringstream 解析一行
////        std::replace(line.begin(), line.end(), ',', ' '); // 把逗号替换为空格
////        std::stringstream ss(line);
////        double x, y, z;
////        if (ss >> x >> y >> z)
////            points.emplace_back(x, y, z);
////    }
////
////    infile.close();
////    return points;
////}
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
//
//int main(int argc, char** argv) {
//    ////////////////////////////fittest////////////////////////////////////////
//       int dim = 3;
//       int order = 4;
//       //std::string filename = "POINTS.DAT"; // 你的文件路径
//       auto ctrl1 = ReadControlPointsFromFile("fittest/POINTS1.txt");
//       std::vector<double> weights1 (25, 1.0);
//       std::vector<double> knots1 = generateUniformKnots(25, 3);//先弄到matlab里面
//
//       // 创建曲线
//       ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//       std::vector<std::vector<double>> Knots = { knots1};
//       std::vector<ON_NurbsCurve> allcurves = { curve1};
//    ///////////////////////////////////paraconsis//////////////////////////////////
//     //int cv_count1 = 5;
//     //  int dim = 3;
//     //  int order = 4;
//     //  //std::string filename = "POINTS.DAT"; // 你的文件路径
//     //  auto ctrl1 = ReadControlPointsFromFile("paraconsis/POINTS1.txt");
//     //  std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 };
//     //  std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.3,1.0, 1.0, 1.0 };//先弄到matlab里面
//
//     //  // 创建曲线
//     //  ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//     //  //PrintNURBSCurveInfo(curve1);
//     //  auto ctrl2 = ReadControlPointsFromFile("paraconsis/POINTS2.txt");
//     //  std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.7,1.0, 1.0, 1.0 };
//     //  // 创建曲线
//     //  ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//     //  auto ctrl3 = ReadControlPointsFromFile("paraconsis/POINTS3.txt");
//     //  std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.4,1.0, 1.0, 1.0 };
//     //  // 创建曲线o
//     //  ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//     //  auto ctrl4 = ReadControlPointsFromFile("paraconsis/POINTS4.txt");
//     //  std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//     //  // 创建曲线
//     //  ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//     //
//     // 
//
//     //  std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4};
//     //  std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4};
//     ///////////////////////////////model6/////////////////////////////////////////
//    //int cv_count1 = 18;
//    //int dim = 3;
//    //int order = 4;
//    ////std::string filename = "POINTS.DAT"; // 你的文件路径
//    //auto ctrl1 = ReadControlPointsFromFile("model6_Info/POINTS1.txt");
//    //if (!ctrl1.empty())
//    //{
//    //    ctrl1.insert(ctrl1.begin(), ctrl1.back());
//    //}
//    //std::vector<double> weights1(cv_count1, 1.0);
//    //std::vector<double> knots1 = generateUniformKnots(cv_count1, 3);
//
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("model6_Info/POINTS2.txt");
//    //if (!ctrl2.empty())
//    //{
//    //    ctrl2.insert(ctrl2.begin(), ctrl2.back());
//    //}
//    //std::vector<double> knots2 = generateUniformKnots(cv_count1, 3);
//
//    //knots2[3] = 0.07;
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//
//    //auto ctrl3 = ReadControlPointsFromFile("model6_Info/POINTS3.txt");
//    //if (!ctrl3.empty())
//    //{
//    //    ctrl3.insert(ctrl3.begin(), ctrl3.back());
//    //}
//    //std::vector<double> knots3 = generateUniformKnots(cv_count1, 3);
//    //// 创建曲线o
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//
//    //auto ctrl4 = ReadControlPointsFromFile("model6_Info/POINTS4.txt");
//    //if (!ctrl4.empty())
//    //{
//    //    ctrl4.insert(ctrl4.begin(), ctrl4.back());
//    //}
//    //std::vector<double> knots4 = generateUniformKnots(cv_count1, 3);
//    //knots4[7] = 0.3;
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//
//    //auto ctrl5 = ReadControlPointsFromFile("model6_Info/POINTS5.txt");
//    //if (!ctrl5.empty())
//    //{
//    //    ctrl5.insert(ctrl5.begin(), ctrl5.back());
//    //}
//    //std::vector<double> knots5 = generateUniformKnots(cv_count1, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//
//    //auto ctrl6 = ReadControlPointsFromFile("model6_Info/POINTS6.txt");
//    //if (!ctrl6.empty())
//    //{
//    //    ctrl6.insert(ctrl6.begin(), ctrl6.back());
//    //}
//    //std::vector<double> knots6 = generateUniformKnots(cv_count1, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 };
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6 };
//    /////////////////////////////////model6/////////////////////////////////////////
//    //   int cv_count1 = 18;
//    //   int dim = 3;
//    //   int order = 4;
//    //   //std::string filename = "POINTS.DAT"; // 你的文件路径
//    //   auto ctrl1 = ReadControlPointsFromFile("model6_Info/POINTS1.txt");
//    //   if (!ctrl1.empty())
//    //   {
//    //       ctrl1.insert(ctrl1.begin(), ctrl1.back());
//    //   }
//    //   std::vector<double> weights1 (cv_count1, 1.0);;
//    //   std::vector<double> knots1 = generateUniformKnots(cv_count1, 3);
//
//    //   // 创建曲线
//    //   ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    //   //PrintNURBSCurveInfo(curve1);
//    //   auto ctrl2 = ReadControlPointsFromFile("model6_Info/POINTS2.txt");
//    //   if (!ctrl2.empty())
//    //   {
//    //       ctrl2.insert(ctrl2.begin(), ctrl2.back());
//    //   }
//    //   std::vector<double> knots2 = generateUniformKnots(cv_count1, 3);
//    //   
//    //   knots2[3] = 0.07;
//    //   // 创建曲线
//    //   ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//
//    //   auto ctrl3 = ReadControlPointsFromFile("model6_Info/POINTS3.txt");
//    //   if (!ctrl3.empty())
//    //   {
//    //       ctrl3.insert(ctrl3.begin(), ctrl3.back());
//    //   }
//    //   std::vector<double> knots3 = generateUniformKnots(cv_count1, 3);
//    //   // 创建曲线o
//    //   ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//
//    //   auto ctrl4 = ReadControlPointsFromFile("model6_Info/POINTS4.txt");
//    //   if (!ctrl4.empty())
//    //   {
//    //       ctrl4.insert(ctrl4.begin(), ctrl4.back());
//    //   }
//    //   std::vector<double> knots4 = generateUniformKnots(cv_count1, 3);
//    //   knots4[7] = 0.3;
//    //   // 创建曲线
//    //   ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//
//    //   auto ctrl5 = ReadControlPointsFromFile("model6_Info/POINTS5.txt");
//    //   if (!ctrl5.empty())
//    //   {
//    //       ctrl5.insert(ctrl5.begin(), ctrl5.back());
//    //   }
//    //   std::vector<double> knots5 = generateUniformKnots(cv_count1, 3);
//    //   // 创建曲线
//    //   ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//
//    //   auto ctrl6 = ReadControlPointsFromFile("model6_Info/POINTS6.txt");
//    //   if (!ctrl6.empty())
//    //   {
//    //       ctrl6.insert(ctrl6.begin(), ctrl6.back());
//    //   }
//    //   std::vector<double> knots6 = generateUniformKnots(cv_count1, 3);
//    //   // 创建曲线
//    //   ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//
//    //   std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6};
//    //   std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6};
//
//    ///////////////////////////////Catamaran///////////////////////////////////////////
//    // int dim = 3;
//    //int order = 4;
//    //int ctrl1num = 18;
//    //int ctrl2num = 15;
//    //int ctrl3num = 16;
//    //int ctrl4num = 13;
//    //int ctrl5num = 17;
//    //int ctrl6num = 40;
//    //int ctrl7num = 50;
//
//    //std::vector<double> weights1(ctrl1num, 1.0);
//    //std::vector<double> weights2(ctrl2num, 1.0);
//    //std::vector<double> weights3(ctrl3num, 1.0);
//    //std::vector<double> weights4(ctrl4num, 1.0);
//    //std::vector<double> weights5(ctrl5num, 1.0);
//    //std::vector<double> weights6(ctrl6num, 1.0);
//    //std::vector<double> weights7(ctrl7num, 1.0);
//    //auto ctrl1 = ReadControlPointsFromFile("Catamaran/POINTS1.txt");
//    //auto knots1 = generateUniformKnots(ctrl1num, 3);
//
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//
//
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("Catamaran/POINTS2.txt");
//    //std::vector<double> knots2 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights2, knots2);
//
//
//    //auto ctrl3 = ReadControlPointsFromFile("Catamaran/POINTS3.txt");
//    //std::vector<double> knots3 = generateUniformKnots(ctrl3num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights3, knots3);
//
//
//    //auto ctrl4 = ReadControlPointsFromFile("Catamaran/POINTS4.txt");
//    //std::vector<double> knots4 = generateUniformKnots(ctrl4num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights4, knots4);
//
//
//    //auto ctrl5 = ReadControlPointsFromFile("Catamaran/POINTS5.txt");
//    //std::vector<double> knots5 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights2, knots5);
//
//
//    //auto ctrl6 = ReadControlPointsFromFile("Catamaran/POINTS6.txt");
//    //std::vector<double> knots6 = generateUniformKnots(ctrl5num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights5, knots6);
//
//
//    //auto ctrl7 = ReadControlPointsFromFile("Catamaran/POINTS7.txt");
//    //std::vector<double> knots7 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights6, knots7);
//
//
//    //auto ctrl8 = ReadControlPointsFromFile("Catamaran/POINTS8.txt");
//    //std::vector<double> knots8 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights6, knots8);
//
//
//    //auto ctrl9 = ReadControlPointsFromFile("Catamaran/POINTS9.txt");
//    //std::vector<double> knots9 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights6, knots9);
//
//
//    //auto ctrl10 = ReadControlPointsFromFile("Catamaran/POINTS10.txt");
//    //std::vector<double> knots10 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights6, knots10);
//
//    //auto ctrl11 = ReadControlPointsFromFile("Catamaran/POINTS11.txt");
//    //std::vector<double> knots11 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve11 = CreateNURBSCurve(dim, true, order, ctrl11, weights6, knots11);
//
//    //auto ctrl12 = ReadControlPointsFromFile("Catamaran/POINTS12.txt");
//    //std::vector<double> knots12 = generateUniformKnots(ctrl6num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve12 = CreateNURBSCurve(dim, true, order, ctrl12, weights6, knots12);
//
//    ////auto ctrl13 = ReadControlPointsFromFile("Catamaran/POINTS13.txt");
//    ////std::vector<double> knots13 = generateUniformKnots(ctrl7num, 3);
//    ////// 创建曲线
//    ////ON_NurbsCurve curve13 = CreateNURBSCurve(dim, true, order, ctrl13, weights7, knots13);
//
//    ////auto ctrl14 = ReadControlPointsFromFile("Catamaran/POINTS14.txt");
//    ////std::vector<double> knots14 = generateUniformKnots(ctrl7num, 3);
//    ////// 创建曲线
//    ////ON_NurbsCurve curve14 = CreateNURBSCurve(dim, true, order, ctrl14, weights7, knots14);
//
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7,knots8,knots9,knots10,knots11,knots12 };
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10,curve11,curve12};
//    //////////////////////////////////////SHIP///////////////////////////////////////////
//    //int dim = 3;
//    //int order = 4;
//    //int ctrl1num = 10;
//    //int ctrl2num = 15;
//    //int ctrl3num = 7;
//    //std::vector<double> weights1(ctrl1num, 1.0);
//    //std::vector<double> weights2(ctrl2num, 1.0);
//    //std::vector<double> weights3(ctrl3num, 1.0);
//    //auto ctrl1 = ReadControlPointsFromFile("ship/POINTS1.txt");
//    //auto knots1 = generateUniformKnots(ctrl1num, 3);
//
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//
//
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("ship/POINTS2.txt");
//    //std::vector<double> knots2 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights2, knots2);
//
//
//    //auto ctrl3 = ReadControlPointsFromFile("ship/POINTS3.txt");
//    //std::vector<double> knots3 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights2, knots3);
//
//
//    //auto ctrl4 = ReadControlPointsFromFile("ship/POINTS4.txt");
//    //std::vector<double> knots4 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights2, knots4);
//
//
//    //auto ctrl5 = ReadControlPointsFromFile("ship/POINTS5.txt");
//    //std::vector<double> knots5 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights2, knots5);
//
//
//    //auto ctrl6 = ReadControlPointsFromFile("ship/POINTS6.txt");
//    //std::vector<double> knots6 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights2, knots6);
//
//
//    //auto ctrl7 = ReadControlPointsFromFile("ship/POINTS7.txt");
//    //std::vector<double> knots7 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights2, knots7);
//
//
//    //auto ctrl8 = ReadControlPointsFromFile("ship/POINTS8.txt");
//    //std::vector<double> knots8 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights2, knots8);
//
//
//    //auto ctrl9 = ReadControlPointsFromFile("ship/POINTS9.txt");
//    //std::vector<double> knots9 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights2, knots9);
//
//
//    //auto ctrl10 = ReadControlPointsFromFile("ship/POINTS10.txt");
//    //std::vector<double> knots10 = generateUniformKnots(ctrl3num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights3, knots10);
//
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7,knots8,knots9,knots10 };
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10};
//    //   ///////////////////////////////////car////////////////////////////////////////////
//    //int dim = 3;
//    //int order = 4;
//    //int ctrl1num = 20;
//    //int ctrl2num = 50;
//    //int ctrl3num = 60;
//    //std::vector<double> weights1(ctrl1num, 1.0);
//    //std::vector<double> weights2(ctrl2num, 1.0);
//    //std::vector<double> weights3(ctrl3num, 1.0);
//    //auto ctrl1 = ReadControlPointsFromFile("car/POINTS1.txt");
//    //auto knots1 = generateUniformKnots(ctrl1num, 3);
//    //
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//
//
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("car/POINTS2.txt");
//    //std::vector<double> knots2 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights2, knots2);
//
//
//    //auto ctrl3 = ReadControlPointsFromFile("car/POINTS3.txt");
//    //std::vector<double> knots3 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights2, knots3);
//
//
//    //auto ctrl4 = ReadControlPointsFromFile("car/POINTS4.txt");
//    //std::vector<double> knots4 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights2, knots4);
//    //for (int i = 0; i < curve4.KnotCount(); ++i)
//    //{
//    //    std::cout << "knot[" << i << "] = " << curve4.Knot(i) << std::endl;
//    //}
//    //int knotIndex = 5;          // 第 6 个节点
//    //double newValue = 0.05;   // 你想要的新值
//    //curve4.SetKnot(knotIndex, newValue);
//    //
//    //auto ctrl5 = ReadControlPointsFromFile("car/POINTS5.txt");
//    //std::vector<double> knots5 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights2, knots5);
//
//
//    //auto ctrl6 = ReadControlPointsFromFile("car/POINTS6.txt");
//    //std::vector<double> knots6 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights2, knots6);
//
//
//    //auto ctrl7 = ReadControlPointsFromFile("car/POINTS7.txt");
//    //std::vector<double> knots7 = generateUniformKnots(ctrl2num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights2, knots7);
//
//
//    //auto ctrl8 = ReadControlPointsFromFile("car/POINTS8.txt");
//    //std::vector<double> knots8 = generateUniformKnots(ctrl3num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights3, knots8);
//
//
//    //auto ctrl9 = ReadControlPointsFromFile("car/POINTS9.txt");
//    //std::vector<double> knots9 = generateUniformKnots(ctrl3num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights3, knots9);
//
//
//    //auto ctrl10 = ReadControlPointsFromFile("car/POINTS10.txt");
//    //std::vector<double> knots10 = generateUniformKnots(ctrl1num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights1, knots10);
//
//    //auto ctrl11 = ReadControlPointsFromFile("car/POINTS11.txt");
//    //std::vector<double> knots11 = generateUniformKnots(ctrl1num, 3);
//    //// 创建曲线
//    //ON_NurbsCurve curve11 = CreateNURBSCurve(dim, true, order, ctrl11, weights1, knots11);
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7,knots8,knots9,knots10,knots11 };
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10,curve11 };
//    ////   ///////////////////////////////////model1////////////////////////////////////////////
//       //int cv_count1 = 7;//控制点数量
//       //int dim = 3;
//       //int order = 4;
//       ////std::string filename = "POINTS.DAT"; // 你的文件路径
//       //auto ctrl1 = ReadControlPointsFromFile("UGfigs/model1/POINTS1.DAT");
//       //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 ,1,1};
//       //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//
//
//       ////PrintNURBSCurveInfo(curve1);
//       //auto ctrl2 = ReadControlPointsFromFile("UGfigs/model1/POINTS2.DAT");
//       //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//
//
//       //auto ctrl3 = ReadControlPointsFromFile("UGfigs/model1/POINTS3.DAT");
//       //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//
//
//       //auto ctrl4 = ReadControlPointsFromFile("UGfigs/model1/POINTS4.DAT");
//       //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//
//
//       //auto ctrl5 = ReadControlPointsFromFile("UGfigs/model1/POINTS5.DAT");
//       //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//
//
//       //auto ctrl6 = ReadControlPointsFromFile("UGfigs/model1/POINTS6.DAT");
//       //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.1,0.5,0.56,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//
//
//       //auto ctrl7 = ReadControlPointsFromFile("UGfigs/model1/POINTS7.DAT");
//       //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//
//
//       //auto ctrl8 = ReadControlPointsFromFile("UGfigs/model1/POINTS8.DAT");
//       //std::vector<double> knots8 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights1, knots8);
//
//
//       //auto ctrl9 = ReadControlPointsFromFile("UGfigs/model1/POINTS9.DAT");
//       //std::vector<double> knots9 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights1, knots9);
//
//
//       //auto ctrl10 = ReadControlPointsFromFile("UGfigs/model1/POINTS10.DAT");
//       //std::vector<double> knots10 = { 0.0, 0.0, 0.0,0.1,0.5,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights1, knots10);
//
//       //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7,knots8,knots9,knots10 };
//       //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10 };
//    ////////////////////////model2/////////////////////////////////////////////////////
//       //int cv_count1 = 5;
//       //int dim = 3;
//       //int order = 4;
//       ////std::string filename = "POINTS.DAT"; // 你的文件路径
//       //auto ctrl1 = ReadControlPointsFromFile("UGfigs/model2/POINTS1.DAT");
//       //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 };
//       //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.3,1.0, 1.0, 1.0 };//先弄到matlab里面
//
//       //// 创建曲线
//       //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//       ////PrintNURBSCurveInfo(curve1);
//       //auto ctrl2 = ReadControlPointsFromFile("UGfigs/model2/POINTS2.DAT");
//       //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.7,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//       //auto ctrl3 = ReadControlPointsFromFile("UGfigs/model2/POINTS3.DAT");
//       //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.3,1.0, 1.0, 1.0 };
//       //// 创建曲线o
//       //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//       //auto ctrl4 = ReadControlPointsFromFile("UGfigs/model2/POINTS4.DAT");
//       //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//       //auto ctrl5 = ReadControlPointsFromFile("UGfigs/model2/POINTS5.DAT");
//       //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.2,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//       //auto ctrl6 = ReadControlPointsFromFile("UGfigs/model2/POINTS6.DAT");
//       //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//       //auto ctrl7 = ReadControlPointsFromFile("UGfigs/model2/POINTS7.DAT");
//       //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//       //// 创建曲线
//       //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//
//       //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7 };
//       //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7};
//
//       /////////////////////////////////////model3////////////////////////////////////
//   //int cv_count1 = 8;//控制点数量
//   //int dim = 3;
//   //int order = 4;
//   ////std::string filename = "POINTS.DAT"; // 你的文件路径
//   //auto ctrl1 = ReadControlPointsFromFile("UGfigs/model3/POINTS1.DAT");
//   //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 ,1,1 ,1};
//   //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.1,0.3,0.5,0.7,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//   //
//   //
//   ////PrintNURBSCurveInfo(curve1);
//   //auto ctrl2 = ReadControlPointsFromFile("UGfigs/model3/POINTS2.DAT");
//   //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.1,0.3,0.5,0.7,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//   //
//   //
//   //auto ctrl3 = ReadControlPointsFromFile("UGfigs/model3/POINTS3.DAT");
//   //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.1,0.2,0.5,0.7,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//   //
//   //
//   //auto ctrl4 = ReadControlPointsFromFile("UGfigs/model3/POINTS4.DAT");
//   //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.1,0.3,0.5,0.7,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//   //
//   //
//   //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4 };
//   //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4 };
//
//   //////////////////////////////model4/////////////////////////////////////////
//   //////
//   //int cv_count1 = 5;//控制点数量
//   //int dim = 3;
//   //int order = 4;
//   ////std::string filename = "POINTS.DAT"; // 你的文件路径
//   //auto ctrl1 = ReadControlPointsFromFile("UGfigs/model4/POINTS1.DAT");
//   //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1};
//   //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//   //
//   //
//   ////PrintNURBSCurveInfo(curve1);
//   //auto ctrl2 = ReadControlPointsFromFile("UGfigs/model4/POINTS2.DAT");
//   //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//   //
//   //
//   //auto ctrl3 = ReadControlPointsFromFile("UGfigs/model4/POINTS3.DAT");
//   //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//   //
//   //
//   //auto ctrl4 = ReadControlPointsFromFile("UGfigs/model4/POINTS4.DAT");
//   //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//   //
//   //auto ctrl5 = ReadControlPointsFromFile("UGfigs/model4/POINTS5.DAT");
//   //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//   //auto ctrl6 = ReadControlPointsFromFile("UGfigs/model4/POINTS6.DAT");
//   //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//   //
//   //
//   //auto ctrl7 = ReadControlPointsFromFile("UGfigs/model4/POINTS7.DAT");
//   //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//   //
//   //
//   //auto ctrl8 = ReadControlPointsFromFile("UGfigs/model4/POINTS8.DAT");
//   //std::vector<double> knots8 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights1, knots8);
//   //
//   //
//   //auto ctrl9 = ReadControlPointsFromFile("UGfigs/model4/POINTS9.DAT");
//   //std::vector<double> knots9 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights1, knots9);
//   //
//   //
//   //auto ctrl10 = ReadControlPointsFromFile("UGfigs/model4/POINTS10.DAT");
//   //std::vector<double> knots10 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve10 = CreateNURBSCurve(dim, true, order, ctrl10, weights1, knots10);
//   //
//   //auto ctrl11 = ReadControlPointsFromFile("UGfigs/model4/POINTS11.DAT");
//   //std::vector<double> knots11 = { 0.0, 0.0, 0.0,0.4,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve11 = CreateNURBSCurve(dim, true, order, ctrl11, weights1, knots11);
//   //
//   //
//   //auto ctrl12 = ReadControlPointsFromFile("UGfigs/model4/POINTS12.DAT");
//   //std::vector<double> knots12 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve12 = CreateNURBSCurve(dim, true, order, ctrl12, weights1, knots12);
//   //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6, knots7,knots8,knots9,knots10,knots11,knots12};
//   //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9,curve10,curve11,curve12 };
//   //// ///////////////////////////////////风扇///////////////////////////////////////
//   //int cv_count1 = 5;//控制点数量
//   //int dim = 3;
//   //int order = 4;
//   ////std::string filename = "POINTS.DAT"; // 你的文件路径
//   //auto ctrl1 = ReadControlPointsFromFile("UGfigs/fan/POINTS1.txt");
//   //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1  };
//   //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//   //
//   //
//   ////PrintNURBSCurveInfo(curve1);
//   //auto ctrl2 = ReadControlPointsFromFile("UGfigs/fan/POINTS2.txt");
//   //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//   //
//   //
//   //auto ctrl3 = ReadControlPointsFromFile("UGfigs/fan/POINTS3.txt");
//   //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//   //
//   //
//   //auto ctrl4 = ReadControlPointsFromFile("UGfigs/fan/POINTS4.txt");
//   //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.6,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//   //
//   //
//   //auto ctrl5 = ReadControlPointsFromFile("UGfigs/fan/POINTS5.txt");
//   //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//   //
//   //
//   //auto ctrl6 = ReadControlPointsFromFile("UGfigs/fan/POINTS6.txt");
//   //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights1, knots6);
//   //
//   //
//   //auto ctrl7 = ReadControlPointsFromFile("UGfigs/fan/POINTS7.txt");
//   //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//   //// 创建曲线
//   //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//   //
//   //
//   //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7 };
//   //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7};
//   //// // ///////////////////////////////////风扇1///////////////////////////////////////
//    //int cv_count1 = 5;//控制点数量5或8
//    //int dim = 3;
//    //int order = 4;
//    ////std::string filename = "POINTS.DAT"; // 你的文件路径
//    //auto ctrl1 = ReadControlPointsFromFile("UGfigs/fan1/POINTS1.txt");
//    //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1 };
//    //std::vector<double> weights2 = { 1.0, 1.0, 1.0, 1.0,1,1,1,1 };
//    //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//
//
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("UGfigs/fan1/POINTS2.txt");
//    //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.2,0.4,0.6,0.8,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights2, knots2);
//
//
//    //auto ctrl3 = ReadControlPointsFromFile("UGfigs/fan1/POINTS3.txt");
//    //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.2,0.4,0.6,0.8,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights2, knots3);
//
//
//    //auto ctrl4 = ReadControlPointsFromFile("UGfigs/fan1/POINTS4.txt");
//    //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.2,0.4,0.6,0.8,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights2, knots4);
//
//
//    //auto ctrl5 = ReadControlPointsFromFile("UGfigs/fan1/POINTS5.txt");
//    //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.2,0.4,0.6,0.8,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights2, knots5);
//
//
//    //auto ctrl6 = ReadControlPointsFromFile("UGfigs/fan1/POINTS6.txt");
//    //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.2,0.4,0.6,0.8,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights2, knots6);
//
//
//    //auto ctrl7 = ReadControlPointsFromFile("UGfigs/fan1/POINTS7.txt");
//    //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights1, knots7);
//
//
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7 };
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7 };
//    ///////////////////////////////////////风扇2///////////////////////////////////////
//    //int cv_count1 = 5;//控制点数量6或5
//    //int dim = 3;
//    //int order = 4;
//    ////std::string filename = "POINTS.DAT"; // 你的文件路径
//    //auto ctrl1 = ReadControlPointsFromFile("UGfigs/fan2/POINTS1.txt");
//    //std::vector<double> weights1 = { 1.0, 1.0, 1.0, 1.0,1,1 };
//    //std::vector<double> weights2 = { 1.0, 1.0, 1.0, 1.0,1 };
//    //std::vector<double> knots1 = { 0.0, 0.0, 0.0,0.3,0.6,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve1 = CreateNURBSCurve(dim, true, order, ctrl1, weights1, knots1);
//    //
//    //
//    ////PrintNURBSCurveInfo(curve1);
//    //auto ctrl2 = ReadControlPointsFromFile("UGfigs/fan2/POINTS2.txt");
//    //std::vector<double> knots2 = { 0.0, 0.0, 0.0,0.2,0.6,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve2 = CreateNURBSCurve(dim, true, order, ctrl2, weights1, knots2);
//    //
//    //
//    //auto ctrl3 = ReadControlPointsFromFile("UGfigs/fan2/POINTS3.txt");
//    //std::vector<double> knots3 = { 0.0, 0.0, 0.0,0.2,0.6,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve3 = CreateNURBSCurve(dim, true, order, ctrl3, weights1, knots3);
//    //
//    //
//    //auto ctrl4 = ReadControlPointsFromFile("UGfigs/fan2/POINTS4.txt");
//    //std::vector<double> knots4 = { 0.0, 0.0, 0.0,0.2,0.6,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve4 = CreateNURBSCurve(dim, true, order, ctrl4, weights1, knots4);
//    //
//    //
//    //auto ctrl5 = ReadControlPointsFromFile("UGfigs/fan2/POINTS5.txt");
//    //std::vector<double> knots5 = { 0.0, 0.0, 0.0,0.2,0.6,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve5 = CreateNURBSCurve(dim, true, order, ctrl5, weights1, knots5);
//    //
//    //
//    //auto ctrl6 = ReadControlPointsFromFile("UGfigs/fan2/POINTS6.txt");
//    //std::vector<double> knots6 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve6 = CreateNURBSCurve(dim, true, order, ctrl6, weights2, knots6);
//    //
//    //
//    //auto ctrl7 = ReadControlPointsFromFile("UGfigs/fan2/POINTS7.txt");
//    //std::vector<double> knots7 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve7 = CreateNURBSCurve(dim, true, order, ctrl7, weights2, knots7);
//    //
//    //auto ctrl8 = ReadControlPointsFromFile("UGfigs/fan2/POINTS8.txt");
//    //std::vector<double> knots8 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve8 = CreateNURBSCurve(dim, true, order, ctrl8, weights2, knots8);
//    //
//    //auto ctrl9 = ReadControlPointsFromFile("UGfigs/fan2/POINTS9.txt");
//    //std::vector<double> knots9 = { 0.0, 0.0, 0.0,0.5,1.0, 1.0, 1.0 };
//    //// 创建曲线
//    //ON_NurbsCurve curve9 = CreateNURBSCurve(dim, true, order, ctrl9, weights2, knots9);
//    //
//    //std::vector<std::vector<double>> Knots = { knots1,knots2,knots3,knots4,knots5,knots6 ,knots7,knots8,knots9};
//    //std::vector<ON_NurbsCurve> allcurves = { curve1,curve2,curve3,curve4,curve5,curve6,curve7,curve8,curve9};
//    std::vector<std::vector<ON_3dPoint>> allCpt;
//    for (const auto& c : allcurves) {
//        allCpt.push_back(getControlPoints(c));
//    }
//    writeMultiCurvesWithCrossLinesToOBJ(allCpt, "controlPoints.obj", false);
//    //PrintNURBSCurveInfo(curve7);
//    // 采样点数，可根据需求增减
//    // 构建两条或多条曲线的采样点
//    std::vector<std::vector<ON_3dPoint>> allSampledCurves;
//    for (int i = 0; i < allcurves.size(); i++)
//    {
//        auto sampled = sampleCurve(allcurves[i], 1000);
//        allSampledCurves.push_back(sampled);
//    }
//    // 写入到OBJ，启用横向连接线
//    writeMultiCurvesWithCrossLinesToOBJ(allSampledCurves, "ug_curves.obj", false);
//    double epsilon = 10e-3; // 误差容忍度
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
//    // 保存所有重采样曲线到一个 OBJ
//    writeMultiCurvesWithCrossLinesToOBJ(
//        AllReSampledCurves,
//        "resample_points.obj",
//        false
//    );
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
//    for (size_t i = 0; i < controlPoints.size(); ++i) {
//        std::cout << "Control points for column " << i << ":" << std::endl;
//        for (size_t j = 0; j < controlPoints[i].size(); ++j) {
//            const ON_3dPoint& pt = controlPoints[i][j];
//            std::cout << "\t(" << pt.x << ", " << pt.y << ", " << pt.z << ")" << std::endl;
//        }
//    }
// 
//    writeMultiCurvesWithCrossLinesToOBJ(
//        controlPoints,
//        "controlPointsfit.obj",
//        false    // 画横向连线（列与列之间的连接线）
//    );
// 
//    std::vector<ON_NurbsCurve> ALLREcurves(allcurves.size());
//    for (size_t i = 0; i < allcurves.size(); ++i) {
//        std::vector<double> weights(controlPoints[i].size(), 1.0);
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
//    saveSurfaceToObjFile(tmesh, allcurves, v_knots, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 70, 70, "ug_tsurface.obj");
//
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
//    //saveSurfaceToObjFile(tmesh111, ALLREcurves, v_knots111, order - 1, degree, 0.0, 1.0, 0.0, 1.0, 150, 150, "ug_tsurface1110.obj");
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
//    std::vector<std::vector<ON_3dPoint>> cp_rows2;
//    for (const auto& row : tmesh111) {
//        cp_rows2.push_back(row.control_points);
//    }
//
//    writeMultiCurvesWithCrossLinesToOBJ(cp_rows2, "tmesh_rows2.obj", true);
//
//    saveSurfaceToObjFile(tmesh111, ALLREcurves, v_knots111, order - 1, degree_v, 0.0, 1.0, 0.0, 1.0, 70,70, "ug_tsurface111.obj");
//
//        ///////////////////曲面光顺///////////////////////////////////////
//    //一、构造引导线
//    //1、在已经处理好的截面线上均匀采样
//    int guide_point_num = 40;//引导线数量
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
//   /* std::cout << "V-direction_pro knot vector: ";
//    for (double kv : v_knots111_pro) std::cout << kv << " ";
//    std::cout << std::endl;*/
//
//    //// 打印矩阵
//    //testBasisMatrix(knotsll, 3, paramsll);
//    //std::vector<std::vector<ON_3dPoint>> cp_rows1;
//    //for (const auto& row : tmesh111_pro) {
//    //    cp_rows1.push_back(row.control_points);
//    //}
//    //
//    //writeMultiCurvesWithCrossLinesToOBJ(cp_rows1, "tmesh_rows1.obj", true);
//
//
//    // 1. 保存更新前的 TMesh（包含所有控制点）
//    //tmesh111_pro[8].v += 0.2;
//    redistributeVParameters(tmesh111_pro);
//
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
//    //std::vector<std::vector<ON_3dPoint>> cp_rows2;
//    //for (const auto& row : tmesh111_pro) {
//    //    cp_rows2.push_back(row.control_points);
//    //}
//
//    //writeMultiCurvesWithCrossLinesToOBJ(cp_rows2, "tmesh_rows2.obj", true);
//    saveSurfaceToObjFile(tmesh111_pro, ALLREcurves, v_knots111_pro_change, order - 1, degree, 0.0, 1.0, 0.0, 1.0, 70,70, "ug_tsurface111_pro.obj");
//   
//    /*computeGlobalMeanCurvature(
//   //     100, 100, tmesh111_pro, ALLREcurves, v_knots111_pro_change,
//   //     3, 3,
//   //     0.0, 1.0,
//   //     0.0, 1.0);*/
//
//   // //printTMesh(tmesh111_pro);
//   // //exportSurfaceRowAtFixedV_toOBJ(
//   // //    tmesh111_pro,
//   // //    ALLREcurves,
//   // //    v_knots111_pro_change,
//   // //    degree,
//   // //    degree_v,
//   // //    0.0, 1.0,
//   // //    0.166667,   // 比如第4行对应的 v
//   // //    200,
//   // //    "surface_row_v11.obj"
//   // //);
//   // //exportSurfaceRowAtFixedV_toOBJ(
//   // //    tmesh111_pro,
//   // //    ALLREcurves,
//   // //    v_knots111_pro_change,
//   // //    degree,
//   // //    degree_v,
//   // //    0.0, 1.0,
//   // //    0.333333,   // 比如第4行对应的 v
//   // //    200,
//   // //    "surface_row_v21.obj"
//   // //);
//   // //exportSurfaceRowAtFixedV_toOBJ(
//   // //    tmesh111_pro,
//   // //    ALLREcurves,
//   // //    v_knots111_pro_change,
//   // //    degree,
//   // //    degree_v,
//   // //    0.0, 1.0,
//   // //    0.666667,   // 比如第4行对应的 v
//   // //    200,
//   // //    "surface_row_v31.obj"
//   // //);
//   // //exportSurfaceRowAtFixedV_toOBJ(
//   // //    tmesh111_pro,
//   // //    ALLREcurves,
//   // //    v_knots111_pro_change,
//   // //    degree,
//   // //    degree_v,
//   // //    0.0, 1.0,
//   // //    0.833333,   // 比如第4行对应的 v
//   // //    200,
//   // //    "surface_row_v41.obj"
//   // //);
//    return 0;
//
//}
