#pragma once

#include "ReparameterizationFunction.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>

class CustomOptimizer
{
private:

    // 被优化的问题
    SkinningOptimizer& optimizer;

    // 最大迭代次数
    int maxIterations;

    // 梯度收敛容差
    double gradientTolerance;

    // 目标函数变化收敛容差,如果两次迭代的目标函数值变化已经非常非常小，也认为收敛。
    double functionTolerance;

    // Backtracking 初始步长
    double initialStepSize;

    // 步长缩小比例,如果当前步长太大，就缩小步长。
    double backtrackingFactor;

    // Armijo 条件参数,要求目标函数至少下降多少，才认为这个步长足够好
    double armijoConstant;

    // 最小允许步长
    double minStepSize;


public:

    // 构造函数
    CustomOptimizer(
        SkinningOptimizer& optimizer
    );


    // 执行优化
    bool Optimize();


private:

    // Backtracking Line Search.返回的是第一次满足Armijo 条件 的α
    double BacktrackingLineSearch(
        const std::vector<double>& x,
        const std::vector<double>& gradient,
        double currentObjective
    );
};