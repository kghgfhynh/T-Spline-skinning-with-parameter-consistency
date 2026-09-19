#pragma once//这个头文件只能被包含一次。这是 C++ 防止重复定义。
//定义里面要用的东西必须放在前面
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "Penaltyfunc.h"//用计算基函数求导的函数
#include "opennurbs.h"
// 计算 Greville Abscissae基于opennurbs
std::vector<double> ComputeGrevillePoints(
    const ON_NurbsCurve& curve);
void TestGreville(const ON_NurbsCurve& curve);



class ReparameterizationFunction
{
public:

    
    ON_NurbsCurve curve;// 原始截面曲线 C_i(t)

    ON_NurbsCurve phi;// 归一化重参数函数:phi(u): [0,1]->[0,1]

    std::vector<double> eps;// IPOPT真正优化变量

    // 真正控制系数
    std::vector<double> beta;

    // 原始曲线参数范围
    double t0;
    double t1;

public:
    ////////////////////////重参数函数模块//////////////////////////
    ReparameterizationFunction();// 默认构造函数

    //初始化重参数化函数
    bool Initialize(
        const ON_NurbsCurve& inputCurve,
        int phiDegree,
        int phiCVCount
    );

    //由优化变量 eps 更新 phi
    //rf.eps = new_eps;
    //rf.UpdatePhiFromEps();
    bool UpdatePhiFromEps(const std::vector<double>& eps);

    //实现 EvaluatePhi()
    double EvaluatePhi(double u);

    // 将归一化参数映射回原始参数
    double MapToOriginalParameter(
        double u);

    //实现导数 phi'(u)
    double EvaluatePhiDerivative(
        double u);

    // 计算原始曲线 C(t) 对原始参数 t 的一阶导数
    ON_3dVector EvaluateOriginalCurveDerivative(
        double t
    );

    //knot 提取
    std::vector<double> GetPhiKnotVector() const;

    //实现计算基函数和求导
    BasisResult EvaluatePhiBasisAndDerivatives(double u) const;

    //建立重参数化曲线评价函数
    ON_3dPoint EvaluateReparameterizedCurve(
        double u);// 
    //第二个输入 u这个不是原始 NURBS 参数，是重参数化函数的输入，范围是[0,1],C(phi(u))
    // 给定新的归一化参数 u，通过重参数函数 ϕ(u) 找到原曲线上的对应参数 t，然后计算原曲线上的三维点。

    // 计算 d phi(u) / d epsilon[k]
    double EvaluatePhiDerivativeWrtEpsilon(
        double u,
        int epsilonIndex
    );

    // 计算重参数化曲线点
// P(u)=C(t(phi(u)))
// 对 epsilon[k] 的偏导数
    ON_3dVector EvaluateReparameterizedCurveDerivativeWrtEpsilon(
        double u,
        int epsilonIndex
    );
};




// 验证重参数化精确性
bool TestReparameterizationExactness(
    ReparameterizationFunction& rf,
    int sampleCount,
    double tolerance);
//测试基函数计算问题
void TestPhiBasisConsistency(
    ReparameterizationFunction& rf,
    double u);
//有限差分和解析对比测试d phi / d epsilon_k
void TestPhiEpsilonGradient(
    ReparameterizationFunction& rf,
    double u,
    int epsilonIndex
);
//有限差分和解析对比测试d C(phi) / d epsilon_k
void TestReparameterizedCurveGradient(
    ReparameterizationFunction& rf,
    double u,
    int epsilonIndex
);


//建立所有截面的统一优化变量管理
class SkinningOptimizer
{
public:

    // 所有截面的重参数函数
    std::vector<ReparameterizationFunction> sections;

    // 所有截面的 epsilon 合并后的全局优化变量
    std::vector<double> eps;

    // 每条截面在全局 eps 中的起始位置
    std::vector<int> epsStartIndex;

    // 总优化变量数量
    int totalVariableCount;

    // 参数采样数量
    int sampleCount;


public:

    //为多截面联合优化建立统一的优化变量向量
    void Initialize();

    //把IPOPT的全局优化变量重新拆回每条截面
    void UpdateAllPhi();

    //蒙皮等参数线光顺能
    double ComputeSkinFairnessEnergy();
    
    //最终目标函数
    double ComputeObjective();

    // 计算目标函数对全局 epsilon 的梯度
    void ComputeObjectiveGradient(
        std::vector<double>& gradient
    );
};

//有限差分和解析对比测试d E / d epsilon_k
void TestSkinFairnessGradient(
    SkinningOptimizer& optimizer);