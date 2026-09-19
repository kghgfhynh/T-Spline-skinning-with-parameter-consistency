#include "pro_cgts.h"
void SavePointsToOBJ_Debug(
    const std::vector<ON_3dPoint>& pts,
    const std::string& filename
)
{
    std::ofstream fout(filename);
    if (!fout.is_open()) return;

    for (const auto& p : pts)
    {
        fout << "v "
            << p.x << " "
            << p.y << " "
            << p.z << "\n";
    }

    fout.close();
}

double ComputeArcLengthHighAccuracy(
    const ON_NurbsCurve& curve,
    double t0,
    double t1,
    int samples = 500
)
{
    double length = 0.0;

    ON_3dPoint prev;
    curve.EvPoint(t0, prev);

    for (int i = 1; i <= samples; ++i)
    {
        double t = t0 + (t1 - t0) * double(i) / samples;

        ON_3dPoint curr;
        curve.EvPoint(t, curr);

        length += prev.DistanceTo(curr);
        prev = curr;
    }

    return length;
}
double ArcLengthFromStart(
    const ON_NurbsCurve& curve,
    double v1,
    double t,
    int samples = 500
)
{
    return ComputeArcLengthHighAccuracy(curve, v1, t, samples);
}
double FindParameterByArcLength_Bisection(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    double targetLength,
    int maxIter = 250
)
{
    double low = v1;
    double high = v2;
    double mid;

    for (int iter = 0; iter < maxIter; ++iter)
    {
        mid = 0.5 * (low + high);

        double len = ArcLengthFromStart(
            curve,
            v1,
            mid
        );

        if (len < targetLength)
            low = mid;
        else
            high = mid;
    }

    return 0.5 * (low + high);
}
std::vector<double> ComputeUniformArcLengthParameters(
    const ON_NurbsCurve& curve,
    double v1,
    double v2,
    int insertCount
)
{
    std::vector<double> result;

    double totalLength =
        ComputeArcLengthHighAccuracy(
            curve,
            v1,
            v2
        );
    //std::cout << "Arc in " << "[" << v1 << "," << v2 << "]:" << totalLength << std::endl;
    for (int k = 1; k <= insertCount; ++k)
    {
        double alpha =
            double(k) / (insertCount + 1);

        double target =
            alpha * totalLength;

        double v =
            FindParameterByArcLength_Bisection(
                curve,
                v1,
                v2,
                target
            );

        result.push_back(v);
    }

    return result;
}

std::vector<TRow> buildTMesh_pro_pro(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt,
    const std::vector<ON_NurbsCurve>& allguidecurves,
    std::vector<std::vector<double>>& Adaptive_knots,
    int degree, 
    const std::vector<std::vector<double>>& allGuideParams)
{
    int G = allguidecurves.size();
   
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
   
            
            std::vector<double> v_up_each(G);
            std::vector<double> v_down_each(G);

            std::vector<ON_3dPoint> sample_points_up(G);
            std::vector<ON_3dPoint> sample_points_down(G);

            // 1️⃣ 每条引导线独立算
            for (int g = 0; g < G; ++g)
            {
                double v1 = allGuideParams[g][i];
                double v2 = allGuideParams[g][i + 1];
                auto v_list =
                    ComputeUniformArcLengthParameters(
                        allguidecurves[g],
                        v1,
                        v2,
                        2
                    );

                v_up_each[g] = v_list[0];
                v_down_each[g] = v_list[1];
                double totalLength0 =
                    ComputeArcLengthHighAccuracy(
                        allguidecurves[g],
                        v1,
                        v_list[0]
                    );
                double totalLength1 =
                    ComputeArcLengthHighAccuracy(
                        allguidecurves[g],
                        v1,
                        v_list[1]
                    );
                //std::cout << "Arc in 1/3" << "[" << v1 << "," << v2 << "]:" << totalLength0<<"and"<< totalLength1 << std::endl;
            }

            // 2️⃣ 用各自 v_i 取点
            for (int g = 0; g < G; ++g)
            {
                allguidecurves[g].EvPoint(v_up_each[g], sample_points_up[g]);
                allguidecurves[g].EvPoint(v_down_each[g], sample_points_down[g]);
            }
            double v11 = para_v_firstcpt[i];
            double v21 = para_v_firstcpt[i + 1];
            //std::cout << "[" << v11 << "," << v21 << "]" << std::endl;
            // 3️⃣ row.v 只作为排序标记
            double v_up =
                v11 + (v21 - v11) / 3.0;

            double v_down =
                v11 + 2.0 * (v21 - v11) / 3.0;
           
            //std::cout << "[[[[[[[" << v_up << "," << v_down << "]]]]]]]" << std::endl;
           




            std::vector<ON_3dPoint> debug_up_pts;
            std::vector<ON_3dPoint> debug_down_pts;

            for (int g = 0; g < G; ++g)
            {
                ON_3dPoint pt_up, pt_down;

                allguidecurves[g].EvPoint(v_up_each[g], pt_up);
                allguidecurves[g].EvPoint(v_down_each[g], pt_down);

                debug_up_pts.push_back(pt_up);
                debug_down_pts.push_back(pt_down);
            }
            std::string filename_up =
                "debug_arc_up_" + std::to_string(i) + ".obj";

            std::string filename_down =
                "debug_arc_down_" + std::to_string(i) + ".obj";

            SavePointsToOBJ_Debug(debug_up_pts, filename_up);
            SavePointsToOBJ_Debug(debug_down_pts, filename_down);


            TRow row_up, row_down;
            row_up.v = v_up;
            row_up.u_positions = grevilleAbscissae(curves[i]);

            //1、找到每条引导线allguidecurves上对应参数位置v_up的点
            int Nu_up = curves[i].CVCount();
            row_up.control_points.resize(Nu_up);//控制点大小
            
            /*       std::string filename_up = "sample_points_up_" + std::to_string(i) + ".obj";
          SavePointsToOBJ_pro(sample_points_up, filename_up);*/
          //2、找到对应的节点向量，反解控制点
            std::vector<double>knot_up = Adaptive_knots[i];
            //std::cout << "knot_up.size() = " << knot_up.size() << std::endl;
            //算采样点对应参数向量
            std::vector<double>para_up(allguidecurves.size());
            para_up[0] = 0.0;
            double total_len = 0.0;

            for (int j = 1; j < sample_points_up.size(); ++j)
            {
                total_len += sample_points_up[j].DistanceTo(sample_points_up[j - 1]);
                para_up[j] = total_len;
            }

            // 归一化到 u_start ~ u_end
            for (int j = 0; j < para_up.size(); ++j)
            {
                para_up[j] = 0 +
                    1 * para_up[j] / total_len;
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
            std::vector<ON_3dPoint>tempora_cpts(knots_insert_up.size() - degree + 1);
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
            ON_NurbsCurve curves_insert_up = CreateNURBSCurve(dim, false, degree + 1, tempora_cpts, weights, knots_insert_up);

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
            int Nu_down = curves[i + 1].CVCount();
            row_down.control_points.resize(Nu_down);//控制点大小
            // 1. 在所有引导线上找 v_up 处的点
           
            //// 生成 sample_points_down 后
            //std::string filename_down = "sample_points_down_" + std::to_string(i) + ".obj";
            //SavePointsToOBJ_pro(sample_points_down, filename_down);
            //2、找到对应的节点向量，反解控制点
            std::vector<double>knot_down = Adaptive_knots[i + 1];
            //算采样点对应参数向量
            std::vector<double>para_down(allguidecurves.size());
            para_down[0] = 0.0;
            double total_lendown = 0.0;

            for (int j = 1; j < sample_points_down.size(); ++j)
            {
                total_lendown += sample_points_down[j].DistanceTo(sample_points_down[j - 1]);
                para_down[j] = total_lendown;
            }

            // 归一化到 u_start ~ u_end
            for (int j = 0; j < para_down.size(); ++j)
            {
                para_down[j] = 0 +
                    1 * para_down[j] / total_lendown;
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

// ------------------------------------------------------------
// 计算两条截线第一个控制点之间的距离
// ------------------------------------------------------------
double rowPositionDistance(
    const TRow& r0,
    const TRow& r1)
{
    // 第一个控制点
    const ON_3dPoint& p0 = r0.control_points.front();
    const ON_3dPoint& p1 = r1.control_points.front();

    // 欧氏距离
    return p0.DistanceTo(p1);
}


// ------------------------------------------------------------
// 计算截线控制多边形的初始切向
// ------------------------------------------------------------
ON_3dVector rowTangent(
    const TRow& r)
{
    // 如果控制点不足两个
    if (r.control_points.size() < 2)
        return ON_3dVector(0, 0, 0);

    // 第一条控制多边形边
    ON_3dVector t =
        r.control_points[1] -
        r.control_points[0];

    t.Unitize();

    return t;
}


// ------------------------------------------------------------
// 计算相邻截线之间的曲率变化量
// ------------------------------------------------------------
double rowCurvature(
    const TRow& r0,
    const TRow& r1)
{
    ON_3dVector t0 = rowTangent(r0);
    ON_3dVector t1 = rowTangent(r1);

    return (t1 - t0).Length();
}


// ------------------------------------------------------------
// 自动重新分布 v 参数
// ------------------------------------------------------------
void redistributeVParameters(
    std::vector<TRow>& tmesh)
{
    int N = tmesh.size();

    std::vector<double> dist(N, 0.0);

    const double lambda = 0;


    // --------------------------------------------------------
    // Step1 计算参数距离
    // --------------------------------------------------------
    for (int i = 1; i < N; i++)
    {
        // 第一控制点距离
        double pos =
            rowPositionDistance(
                tmesh[i - 1],
                tmesh[i]);

        // 切向变化
        double curv =
            rowCurvature(
                tmesh[i - 1],
                tmesh[i]);

        // centripetal + curvature
        dist[i] =
            cbrt(pos)
            + lambda * curv;
    }


    // --------------------------------------------------------
    // Step2 平滑 dist
    // --------------------------------------------------------
    for (int i = 2; i < N - 1; i++)
    {
        dist[i] =
            (dist[i - 1] + 2.0 * dist[i] + dist[i + 1]) / 4.0;
    }


    // --------------------------------------------------------
    // Step3 计算总距离
    // --------------------------------------------------------
    double total = 0.0;

    for (int i = 1; i < N; i++)
        total += dist[i];


    // --------------------------------------------------------
    // Step4 生成 v 参数
    // --------------------------------------------------------
    double accum = 0.0;

    tmesh[0].v = 0.0;

    for (int i = 1; i < N; i++)
    {
        accum += dist[i];

        tmesh[i].v = accum / total;
    }
}






