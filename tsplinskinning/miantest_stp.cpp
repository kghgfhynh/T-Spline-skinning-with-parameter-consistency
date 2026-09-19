//#include "readstp.h"
//
//int main()
//{
//    // ---------- 1. 读取 STEP ----------
//    STEPControl_Reader reader;
//
//    IFSelect_ReturnStatus status =
//        reader.ReadFile("model2.stp");//读取文件 → 返回状态码
//
//    if (status != IFSelect_RetDone)
//    {
//        std::cerr << "STEP read failed\n";
//        return -1;
//    }
//
//    reader.TransferRoots();//将 STEP 中的拓扑结构转换为 OCC 内部 BRep
//    TopoDS_Shape shape = reader.OneShape();//得到统一拓扑体（可能包含多个 face）
//
//    // ---------- 2. 遍历 Face ----------
//    TopExp_Explorer explorer(shape, TopAbs_FACE);//遍历所有面
////
//    int face_id = 0;
//
//
//    for (; explorer.More(); explorer.Next(), ++face_id)
//    {
//        TopoDS_Face face =
//            TopoDS::Face(explorer.Current());//当前拓扑面
//
//        // ---------- 3. 获取 Surface ----------
//        Handle(Geom_Surface) surf =
//            BRep_Tool::Surface(face);//获取几何曲面
//
//        Handle(Geom_BSplineSurface) bs =
//            Handle(Geom_BSplineSurface)::DownCast(surf);//尝试直接转 BSpline
//
//        // 非BSpline → 转换
//        if (bs.IsNull())
//        {
//            bs = GeomConvert::SurfaceToBSplineSurface(surf);//非 BSpline → 强制转换
//        }
//
//        // ---------- 4. 提取 ----------
//        std::vector<ON_NurbsCurve> curves;
//        std::vector<double> v_knots;
//        std::vector<double> v_params;
//
//        ExtractIsoVCurvesFromSurface(
//            bs, curves, v_knots, v_params);
//        PrintNURBSCurveInfo(curves[0]);
//        PrintNURBSCurveInfo(curves[1]);
//        PrintNURBSCurveInfo(curves[2]);
//        PrintNURBSCurveInfo(curves[3]);
//        PrintNURBSCurveInfo(curves[4]);
//        std::vector<TRow> stpmesh = buildTMeshFromCurves(curves,v_params);
//        // 打印T样条网格
//        printTMesh(stpmesh);
//
//        // ---------- 5. 输出 ----------
//        std::cout << "Face " << face_id << "\n";
//        std::cout << "  #curves = " << curves.size() << "\n";
//
//        if (!curves.empty())
//        {
//            std::cout << "  ctrl per curve = "
//                << curves[0].CVCount() << "\n";
//        }
//
//        std::cout << "  v_knots size = "
//            << v_knots.size() << "\n";
//
//        std::cout << "  v_params size = "
//            << v_params.size() << "\n";
//
//        std::cout << "----------------------\n";
//        // ---------- 输出归一化 V knot ----------
//      
//        std::cout << "V-direction_pro knot vector: ";
//        for (double kv : v_knots) std::cout << kv << " ";
//        std::cout << std::endl;
//        computeGlobalMeanCurvature_stp(
//                200, 200, stpmesh,curves, v_knots,
//                3, 3,
//                0.0, 1.0,
//                0.0, 1.0);
//    }
//    
//    return 0;
//}