//
//#include "CustomOptimizer.h"
//#include "readfile.h"
//
//#include <iostream>
//#include <iomanip>
//#include <vector>
//#include <cmath>
//
//int main(int argc, char** argv)
//{
//    // ============================================================
//    // 1. 创建第1条截面
//    // ============================================================
//
//    auto ctrl1 =
//        ReadControlPointsFromFile(
//            "paraconsis/POINTS1.txt");
//
//    std::vector<double> weights1 =
//    {
//        1.0, 1.0, 1.0, 1.0, 1.0
//    };
//
//    std::vector<double> knots1 =
//    {
//        0.1, 0.1, 0.1,
//        0.3,
//        1.0, 1.0, 1.0
//    };
//
//    ON_NurbsCurve curve1 =
//        CreateNURBSCurve(
//            3,
//            true,
//            4,
//            ctrl1,
//            weights1,
//            knots1);
//
//
//    // ============================================================
//    // 2. 创建第2条截面
//    // ============================================================
//
//    auto ctrl2 =
//        ReadControlPointsFromFile(
//            "paraconsis/POINTS2.txt");
//
//    std::vector<double> knots2 =
//    {
//        0.0, 0.0, 0.0,
//        0.7,
//        1.0, 1.0, 1.0
//    };
//
//    ON_NurbsCurve curve2 =
//        CreateNURBSCurve(
//            3,
//            true,
//            4,
//            ctrl2,
//            weights1,
//            knots2);
//
//
//    // ============================================================
//    // 3. 创建第3条截面
//    // ============================================================
//
//    auto ctrl3 =
//        ReadControlPointsFromFile(
//            "paraconsis/POINTS3.txt");
//
//    std::vector<double> knots3 =
//    {
//        0.0, 0.0, 0.0,
//        0.4,
//        1.0, 1.0, 1.0
//    };
//
//    ON_NurbsCurve curve3 =
//        CreateNURBSCurve(
//            3,
//            true,
//            4,
//            ctrl3,
//            weights1,
//            knots3);
//
//
//    // ============================================================
//    // 4. 创建第4条截面
//    // ============================================================
//
//    auto ctrl4 =
//        ReadControlPointsFromFile(
//            "paraconsis/POINTS4.txt");
//
//    std::vector<double> knots4 =
//    {
//        0.0, 0.0, 0.0,
//        0.5,
//        1.0, 1.0, 1.0
//    };
//
//    ON_NurbsCurve curve4 =
//        CreateNURBSCurve(
//            3,
//            true,
//            4,
//            ctrl4,
//            weights1,
//            knots4);
//
//
//    // ============================================================
//    // 5. 创建4个重参数化函数
//    // ============================================================
//
//    ReparameterizationFunction rf1;
//    ReparameterizationFunction rf2;
//    ReparameterizationFunction rf3;
//    ReparameterizationFunction rf4;
//
//
//    // ============================================================
//    // 6. 设置重参数函数的阶次和控制点数量
//    // ============================================================
//
//    int phiDegree = 3;
//    int phiCVCount = 5;
//
//
//    // ============================================================
//    // 7. 初始化4条重参数函数
//    // ============================================================
//
//    if (!rf1.Initialize(
//        curve1,
//        phiDegree,
//        phiCVCount))
//    {
//        std::cerr
//            << "Failed to initialize rf1."
//            << std::endl;
//
//        return 1;
//    }
//
//
//    if (!rf2.Initialize(
//        curve2,
//        phiDegree,
//        phiCVCount))
//    {
//        std::cerr
//            << "Failed to initialize rf2."
//            << std::endl;
//
//        return 1;
//    }
//
//
//    if (!rf3.Initialize(
//        curve3,
//        phiDegree,
//        phiCVCount))
//    {
//        std::cerr
//            << "Failed to initialize rf3."
//            << std::endl;
//
//        return 1;
//    }
//
//
//    if (!rf4.Initialize(
//        curve4,
//        phiDegree,
//        phiCVCount))
//    {
//        std::cerr
//            << "Failed to initialize rf4."
//            << std::endl;
//
//        return 1;
//    }
//
//
//    // ============================================================
//    // 8. 创建 SkinningOptimizer
//    // ============================================================
//
//    SkinningOptimizer optimizer;
//
//
//    // ============================================================
//    // 9. 将4条截面的重参数函数加入 sections
//    // ============================================================
//
//    optimizer.sections.push_back(rf1);
//    optimizer.sections.push_back(rf2);
//    optimizer.sections.push_back(rf3);
//    optimizer.sections.push_back(rf4);
//
//
//    // ============================================================
//    // 10. 初始化全局优化变量
//    // ============================================================
//
//    optimizer.Initialize();
//
//
//    std::cout
//        << "========================================"
//        << std::endl;
//
//    std::cout
//        << "Skinning optimization problem"
//        << std::endl;
//
//    std::cout
//        << "Number of sections = "
//        << optimizer.sections.size()
//        << std::endl;
//
//    std::cout
//        << "Sample count = "
//        << optimizer.sampleCount
//        << std::endl;
//
//    std::cout
//        << "Number of variables = "
//        << optimizer.totalVariableCount
//        << std::endl;
//
//    std::cout
//        << "========================================"
//        << std::endl;
//
//
//    // ============================================================
//    // 11. 检查每条截面的 epsilon 数量
//    // ============================================================
//
//    for (size_t i = 0;
//        i < optimizer.sections.size();
//        ++i)
//    {
//        std::cout
//            << "Section "
//            << i
//            << " : epsilon count = "
//            << optimizer.sections[i].eps.size()
//            << std::endl;
//    }
//
//
//    // ============================================================
//    // 12. 输出初始 epsilon
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========== Initial Epsilon =========="
//        << std::endl;
//
//    std::cout
//        << std::setprecision(15);
//
//    for (size_t k = 0;
//        k < optimizer.eps.size();
//        ++k)
//    {
//        std::cout
//            << "eps["
//            << k
//            << "] = "
//            << optimizer.eps[k]
//            << std::endl;
//    }
//
//
//    // ============================================================
//    // 13. 计算初始目标函数
//    // ============================================================
//
//    optimizer.UpdateAllPhi();
//
//    double initialObjective =
//        optimizer.ComputeObjective();
//
//
//    std::cout
//        << std::endl
//        << "Initial Objective = "
//        << initialObjective
//        << std::endl;
//
//
//    // ============================================================
//    // 14. 测试解析梯度
//    //
//    // 这一项之前已经通过有限差分验证。
//    // 保留在这里，方便以后检查。
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========== Gradient Verification =========="
//        << std::endl;
//
//    TestSkinFairnessGradient(
//        optimizer);
//
//
//    // ============================================================
//    // 15. 输出优化前的截面点
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========== Initial Section Points =========="
//        << std::endl;
//
//    double u = 0.5;
//
//    for (size_t i = 0;
//        i < optimizer.sections.size();
//        ++i)
//    {
//        ON_3dPoint P =
//            optimizer.sections[i]
//            .EvaluateReparameterizedCurve(u);
//
//        std::cout
//            << "Section "
//            << i
//            << " : "
//            << P.x << " "
//            << P.y << " "
//            << P.z
//            << std::endl;
//    }
//    std::cout
//        << std::endl
//        << "========== initial Epsilon =========="
//        << std::endl;
//
//    for (size_t k = 0;
//        k < optimizer.eps.size();
//        ++k)
//    {
//        std::cout
//            << "eps["
//            << k
//            << "] = "
//            << optimizer.eps[k]
//            << std::endl;
//    }
//
//    // ============================================================
//    // 16. 创建自己的优化器
//    // ============================================================
//
//    CustomOptimizer solver(
//        optimizer);
//
//
//    // ============================================================
//    // 17. 开始优化
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========================================"
//        << std::endl;
//
//    std::cout
//        << "Start Custom Optimization"
//        << std::endl;
//
//    std::cout
//        << "========================================"
//        << std::endl;
//
//
//    bool success =
//        solver.Optimize();
//
//
//    // ============================================================
//    // 18. 输出优化状态
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========================================"
//        << std::endl;
//
//    if (success)
//    {
//        std::cout
//            << "Optimization finished successfully."
//            << std::endl;
//    }
//    else
//    {
//        std::cout
//            << "Optimization did not converge."
//            << std::endl;
//    }
//
//
//    // ============================================================
//    // 19. 计算最终目标函数
//    // ============================================================
//
//    double finalObjective =
//        optimizer.ComputeObjective();
//
//
//    std::cout
//        << std::setprecision(15)
//        << "Initial Objective = "
//        << initialObjective
//        << std::endl;
//
//    std::cout
//        << "Final Objective   = "
//        << finalObjective
//        << std::endl;
//
//    std::cout
//        << "Objective reduction = "
//        << initialObjective - finalObjective
//        << std::endl;
//
//
//    // ============================================================
//    // 20. 输出最终 epsilon
//    // ============================================================
//
//    std::cout
//        << std::endl
//        << "========== Final Epsilon =========="
//        << std::endl;
//
//    for (size_t k = 0;
//        k < optimizer.eps.size();
//        ++k)
//    {
//        std::cout
//            << "eps["
//            << k
//            << "] = "
//            << optimizer.eps[k]
//            << std::endl;
//    }
//
//
//    
//    return 0;
//}