#include "CustomOptimizer.h"

CustomOptimizer::CustomOptimizer(
    SkinningOptimizer& optimizer)
    : optimizer(optimizer)
{
    maxIterations = 1000;

    gradientTolerance = 1e-6;

    functionTolerance = 1e-3;

    // 与你原来的 0.001 不同
    // Line Search 从 1.0 开始自动寻找
    initialStepSize = 0.7;

    // 每次失败后：
    // alpha = alpha * 0.5
    backtrackingFactor = 0.8;

    // Armijo 参数
    armijoConstant = 1e-4;

    // 防止 alpha 无限缩小
    minStepSize = 1e-12;
}

double CustomOptimizer::BacktrackingLineSearch(
    const std::vector<double>& x,
    const std::vector<double>& gradient,
    double currentObjective)
{
    // ----------------------------------------
    // 1. 初始步长
    // ----------------------------------------

    double alpha = initialStepSize;


    // ----------------------------------------
    // 2. 计算 ||gradient||^2计算梯度平方范数
    // ----------------------------------------

    double gradientNormSquared = 0.0;

    for (double g : gradient)
    {
        gradientNormSquared += g * g;
    }


    // ----------------------------------------
    // 3. 梯度下降方向
    //
    // d = -gradient
    // ----------------------------------------

    while (alpha >= minStepSize)
    {
        // ------------------------------------
        // 构造 trial point
        //
        // x_trial = x - alpha * gradient
        // ------------------------------------

        std::vector<double> trialX = x;

        for (size_t k = 0;
            k < trialX.size();
            ++k)
        {
            trialX[k]
                -= alpha * gradient[k];
        }


        // ------------------------------------
        // 暂时把 trialX 放入 optimizer
        // ------------------------------------

        optimizer.eps = trialX;

        // 根据新的 epsilon 更新所有 phi
        optimizer.UpdateAllPhi();


        // ------------------------------------
        // 计算 trial objective
        // ------------------------------------

        double trialObjective =
            optimizer.ComputeObjective();


        // ------------------------------------
        // Armijo 条件
        //
        // f(x + alpha*d)
        // <=
        // f(x) + c*alpha*g^T*d
        //
        // 因为 d = -g
        //
        // g^T*d = -||g||²
        //
        // 所以：
        //
        // trialObjective
        // <=
        // currentObjective
        // -
        // c*alpha*||g||²
        // ------------------------------------

        double rightSide =
            currentObjective
            - armijoConstant
            * alpha
            * gradientNormSquared;


        if (trialObjective <= rightSide)
        {
            // --------------------------------
            // 找到了满足 Armijo 条件的步长
            // --------------------------------

            return alpha;
        }


        // ------------------------------------
        // 不满足条件
        // 缩小步长
        // ------------------------------------

        alpha *= backtrackingFactor;
    }


    // ----------------------------------------
    // 没找到合适步长
    // ----------------------------------------

    return 0.0;
}


bool CustomOptimizer::Optimize()
{
    std::cout
        << "========================================"
        << std::endl;

    std::cout
        << "Start optimization"
        << std::endl;

    std::cout
        << "Number of variables = "
        << optimizer.totalVariableCount
        << std::endl;

    std::cout
        << "Maximum iterations = "
        << maxIterations
        << std::endl;

    std::cout
        << "Initial step size = "
        << initialStepSize
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;


    // ----------------------------------------
    // 初始变量
    // ----------------------------------------

    std::vector<double> x =
        optimizer.eps;


    // 确保 phi 与 x 一致
    optimizer.UpdateAllPhi();


    // ----------------------------------------
    // 初始目标函数
    // ----------------------------------------

    double objective =
        optimizer.ComputeObjective();


    // ----------------------------------------
    // 开始迭代
    // ----------------------------------------

    for (int iteration = 0;
        iteration < maxIterations;
        ++iteration)
    {
        // ------------------------------------
        // 1. 计算梯度
        // ------------------------------------

        std::vector<double> gradient;

        optimizer.ComputeObjectiveGradient(
            gradient);


        // ------------------------------------
        // 2. 计算梯度范数
        // ------------------------------------

        double gradientNormSquared = 0.0;

        for (double g : gradient)
        {
            gradientNormSquared += g * g;
        }

        double gradientNorm =
            std::sqrt(gradientNormSquared);


        // ------------------------------------
        // 3. 输出
        // ------------------------------------

        std::cout
            << "Iteration "
            << iteration
            << " : Objective = "
            << std::setprecision(10)
            << objective
            << " , GradientNorm = "
            << gradientNorm
            << std::endl;


        // ------------------------------------
        // 4. 梯度收敛判断
        // ------------------------------------

        if (gradientNorm < gradientTolerance)
        {
            std::cout
                << "Optimization converged."
                << std::endl;

            std::cout
                << "Final iteration = "
                << iteration
                << std::endl;

            return true;
        }


        // ------------------------------------
        // 5. 保存当前变量
        // ------------------------------------

        std::vector<double> oldX = x;

        double oldObjective =
            objective;


        // ------------------------------------
        // 6. Line Search
        // ------------------------------------

        double alpha =
            BacktrackingLineSearch(
                oldX,
                gradient,
                oldObjective);


        // ------------------------------------
        // 7. 判断 Line Search 是否成功
        // ------------------------------------

        if (alpha <= 0.0)
        {
            // 恢复旧变量

            optimizer.eps = oldX;

            optimizer.UpdateAllPhi();


            std::cout
                << "Line search failed."
                << std::endl;

            return false;
        }


        // ------------------------------------
        // 8. 接受新变量
        //
        // x_new = x - alpha * gradient
        // ------------------------------------

        x = oldX;

        for (size_t k = 0;
            k < x.size();
            ++k)
        {
            x[k]
                -= alpha * gradient[k];
        }


        // ------------------------------------
        // 9. 更新 optimizer
        // ------------------------------------

        optimizer.eps = x;

        optimizer.UpdateAllPhi();


        // ------------------------------------
        // 10. 重新计算目标函数
        // ------------------------------------

        objective =
            optimizer.ComputeObjective();


        // ------------------------------------
        // 11. 目标函数变化收敛判断
        // ------------------------------------

        double functionChange =
            std::abs(
                objective -
                oldObjective);


        if (functionChange
            <
            functionTolerance
            * (1.0 + std::abs(oldObjective)))
        {
            std::cout
                << "Optimization converged."
                << std::endl;

            std::cout
                << "Function change = "
                << functionChange
                << std::endl;

            return true;
        }
    }


    // ----------------------------------------
    // 达到最大迭代次数
    // ----------------------------------------

    std::cout
        << "Optimization reached maximum iterations."
        << std::endl;

    return false;
}