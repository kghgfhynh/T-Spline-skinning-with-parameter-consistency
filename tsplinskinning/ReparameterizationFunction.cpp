#include "ReparameterizationFunction.h"

/////////////////////////////////////阶段一///////////////////////////////////////
std::vector<double> ComputeGrevillePoints(
    const ON_NurbsCurve& curve)
{
  
    int degree = curve.Degree();// 得到次数 p

    int cvCount = curve.CVCount();// 得到控制点个数

    std::vector<double> greville(cvCount);// 创建Greville数组

    for (int i = 0; i < cvCount; i++)// 对每个控制点计算Greville参数
    {
        double sum = 0;

        for (int j = 0; j < degree; j++)
        {
            sum += curve.Knot(i + j);
        }

        greville[i] = sum / degree;
    }

    return greville;
}

void TestGreville(const ON_NurbsCurve& curve)
{
    auto greville = ComputeGrevillePoints(curve);

    int degree = curve.Degree();

    std::cout << "\n========== Greville Test ==========\n";

    for (int i = 0; i < curve.CVCount(); i++)
    {
        double sum = 0.0;

        std::cout << "Greville[" << i << "] = (";

        for (int j = 0; j < degree; j++)
        {
            double knot = curve.Knot(i + j);

            sum += knot;

            std::cout << knot;

            if (j != degree - 1)
                std::cout << " + ";
        }

        std::cout << ") / " << degree;

        std::cout << " = " << greville[i] << std::endl;
    }
}
//----------------------------------------------------
// 验证重参数化是否保持原始曲线几何精确不变
//----------------------------------------------------
bool TestReparameterizationExactness(
    ReparameterizationFunction& rf,
    int sampleCount,
    double tolerance)
{
    if (sampleCount < 2)
    {
        std::cout
            << "Error: sampleCount must be at least 2."
            << std::endl;

        return false;
    }

    std::cout
        << "========================================"
        << std::endl;

    std::cout
        << "Test Reparameterization Exactness"
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;


    //------------------------------------------------
    // 1. 检查原始曲线
    //------------------------------------------------

    if (!rf.curve.IsValid())
    {
        std::cout
            << "Error: original curve is invalid."
            << std::endl;

        return false;
    }


    //------------------------------------------------
    // 2. 输出原始参数范围
    //------------------------------------------------

    std::cout
        << "Original curve domain: ["
        << rf.t0
        << ", "
        << rf.t1
        << "]"
        << std::endl;


    //------------------------------------------------
    // 3. 开始采样
    //------------------------------------------------

    double maxError = 0.0;
    double averageError = 0.0;

    double maxU = 0.0;
    double maxPhi = 0.0;
    double maxT = 0.0;

    int failedCount = 0;


    std::cout
        << std::scientific
        << std::setprecision(15);


    //------------------------------------------------
    // 4. 对 [0,1] 进行均匀采样
    //------------------------------------------------

    for (int i = 0; i < sampleCount; ++i)
    {
        double u =
            static_cast<double>(i)
            / static_cast<double>(sampleCount - 1);


        //------------------------------------------------
        // A. 计算重参数函数 phi(u)
        //------------------------------------------------

        double phi =
            rf.EvaluatePhi(u);


        //------------------------------------------------
        // B. 将归一化参数 phi(u)
        //    映射到原始 NURBS 参数 t
        //------------------------------------------------

        double t =
            rf.MapToOriginalParameter(phi);


        //------------------------------------------------
        // C. 直接在原始 NURBS 曲线上计算点
        //------------------------------------------------

        ON_3dPoint P_direct =
            rf.curve.PointAt(t);

        //------------------------------------------------
        // D. 使用你的重参数化曲线评价函数
        //------------------------------------------------

        ON_3dPoint P_reparam =
            rf.EvaluateReparameterizedCurve(u);


        //------------------------------------------------
        // E. 计算两点之间的误差
        //------------------------------------------------

        double error =
            P_direct.DistanceTo(P_reparam);


        //------------------------------------------------
        // F. 累加平均误差
        //------------------------------------------------

        averageError += error;


        //------------------------------------------------
        // G. 更新最大误差
        //------------------------------------------------

        if (error > maxError)
        {
            maxError = error;

            maxU = u;
            maxPhi = phi;
            maxT = t;
        }


        //------------------------------------------------
        // H. 判断当前点是否通过
        //------------------------------------------------

        if (error > tolerance)
        {
            failedCount++;
        }
    }


    //------------------------------------------------
    // 5. 计算平均误差
    //------------------------------------------------

    averageError /=
        static_cast<double>(sampleCount);


    //------------------------------------------------
    // 6. 输出最终结果
    //------------------------------------------------

    std::cout
        << std::endl;

    std::cout
        << "Number of samples       = "
        << sampleCount
        << std::endl;

    std::cout
        << "Tolerance               = "
        << tolerance
        << std::endl;

    std::cout
        << "Maximum geometric error = "
        << maxError
        << std::endl;

    std::cout
        << "Average geometric error = "
        << averageError
        << std::endl;

    std::cout
        << "Maximum error occurs at:"
        << std::endl;

    std::cout
        << "    u   = "
        << maxU
        << std::endl;

    std::cout
        << "    phi = "
        << maxPhi
        << std::endl;

    std::cout
        << "    t   = "
        << maxT
        << std::endl;

    std::cout
        << "Failed samples          = "
        << failedCount
        << std::endl;


    //------------------------------------------------
    // 7. 最终判断
    //------------------------------------------------

    if (maxError <= tolerance)
    {
        std::cout
            << std::endl
            << "PASS:"
            << std::endl;

        std::cout
            << "Reparameterization preserves "
            << "the original curve geometry."
            << std::endl;

        return true;
    }
    else
    {
        std::cout
            << std::endl
            << "FAIL:"
            << std::endl;

        std::cout
            << "The reparameterized curve "
            << "does not match the original curve "
            << "within the specified tolerance."
            << std::endl;

        return false;
    }
}

//构造函数。如果需要默认设置就在这里写。
ReparameterizationFunction::ReparameterizationFunction()
{
}

bool ReparameterizationFunction::Initialize(
    const ON_NurbsCurve& inputCurve,//输入一条原始截面曲线
    int phiDegree,
    int phiCVCount)//控制系数数量
{
  // 保存原始曲线
    curve = inputCurve;

    // 保存参数范围

    t0 = curve.Domain().Min();
    t1 = curve.Domain().Max();

    // 1. 创建一条一维 B 样条

    bool rc = phi.Create(
        1,                  // dimension = 1
        false,              // 非有理 B样条
        phiDegree + 1,      // order = degree + 1
        phiCVCount);        // 控制点个数

    if (!rc)
    {
        std::cout << "Create phi failed!" << std::endl;
        return false;
    }

    // 2. 设置节点向量,假设重参数函数的参数范围是[0,1]

    std::vector<double> knots =
        generateUniformKnots(
            phiCVCount,
            phiDegree);

    for (int i = 0; i < phi.KnotCount(); i++)
    {
        phi.SetKnot(i, knots[i]);
    }
  // 3. 计算 Greville 点

    std::vector<double> greville =
        ComputeGrevillePoints(phi);

    if (greville.size() != phiCVCount)
    {
        std::cout
            << "Greville size error!"
            << std::endl;

        return false;
    }

   // 4.初始化 beta
   //
   // beta_i = ξ_i
   //
   // phi初始为单位映射

    beta.resize(phiCVCount);

    for (int i = 0; i < phiCVCount; i++)
    {
        beta[i] = greville[i];
    }

    //  检查 beta 是否满足 [0,1] 和单调性

    if (std::abs(beta[0]) > 1e-12)
    {
        std::cout
            << "Error: beta[0] is not zero."
            << std::endl;

        return false;
    }

    if (std::abs(beta[phiCVCount - 1] - 1.0) > 1e-12)
    {
        std::cout
            << "Error: beta[last] is not one."
            << std::endl;

        return false;
    }

    for (int i = 1; i < phiCVCount; ++i)
    {
        if (beta[i] < beta[i - 1])
        {
            std::cout
                << "Error: beta is not monotonic."
                << std::endl;

            return false;
        }
    }
     // 5. 根据 beta 反求 eps
     //
     // ε0²也是优化对象，不等于β0 ，而对应β1
     //
     // βi+1=βi+εi²

    eps.resize(phiCVCount-1);
   
    for (int i = 1; i < phiCVCount; ++i)
    {
        double diff =
            beta[i] - beta[i - 1];

        eps[i - 1] =
            std::sqrt(diff);
    }
        // 6. 设置 phi 控制点
        //
        // phi(u)=ΣNi(u)βi

    for (int i = 0; i < phiCVCount; i++)
    {

        double value = beta[i];


        phi.SetCV(
            i,
            ON::not_rational,
            &value);
    }



    std::cout << "phi degree = "
        << phi.Degree() << std::endl;

    std::cout << "phi CV count = "
        << phi.CVCount() << std::endl;

    std::cout << "phi knot count = "
        << phi.KnotCount() << std::endl;
    std::cout << "Knots:\n";

    for (int i = 0; i < phi.KnotCount(); i++)
    {
        std::cout
            << phi.Knot(i)
            << " ";
    }

    std::cout << std::endl;
    std::cout << "\nControl coefficients:\n";

    for (int i = 0; i < phi.CVCount(); i++)
    {
        double cv;

        phi.GetCV(
            i,
            ON::not_rational,
            &cv);

        std::cout
            << i
            << " : "
            << cv
            << std::endl;
    }



    return true;
}

double ReparameterizationFunction::MapToOriginalParameter(double u)
{
    double s;

    phi.Evaluate(
        u,
        0,
        1,
        &s);

    // s ∈ [0,1] 映射到：t ∈ [t0,t1]

    double t = t0 + (t1 - t0) * s;

    return t;
}

bool ReparameterizationFunction::UpdatePhiFromEps(
    const std::vector<double>& newEps)
{
    int cvCount = phi.CVCount();

    // 真正优化变量数量 = 控制点数量 - 1
    if (static_cast<int>(newEps.size()) != cvCount - 1)
    {
        return false;
    }

    beta.resize(cvCount);

    // β0 = 0
    beta[0] = 0.0;
    // 保存当前真正的优化变量
    eps = newEps;
    // ------------------------------------------
    // 1. 计算累计平方
    // ------------------------------------------

    double total = 0.0;

    for (int i = 0; i < cvCount - 1; ++i)
    {
        total += eps[i] * eps[i];
    }

    // 防止所有 epsilon 都为 0
    if (total <= 1e-14)
    {
        return false;
    }

    // ------------------------------------------
    // 2. 归一化得到 beta
    // ------------------------------------------

    double cumulative = 0.0;

    for (int i = 1; i < cvCount; ++i)
    {
        cumulative +=
            eps[i - 1] * eps[i - 1];

        beta[i] =
            cumulative / total;
    }

    // ------------------------------------------
    // 3. beta -> phi control points
    // ------------------------------------------

    for (int i = 0; i < cvCount; ++i)
    {
        double value = beta[i];

        phi.SetCV(
            i,
            ON::not_rational,
            &value
        );
    }

    return true;
}

double ReparameterizationFunction::EvaluatePhi(
    double u)
{
    double value;

    bool rc =
        phi.Evaluate(
            u,
            0,
            1,
            &value);


    if (!rc)
    {
        std::cout
            << "Evaluate phi failed!"
            << std::endl;

        return 0.0;
    }

    return value;
}

double ReparameterizationFunction::EvaluatePhiDerivative(
    double u)
{
    double v[2];

    bool rc =
        phi.Evaluate(
            u,
            1,
            1,
            v);

    if (!rc)
    {
        return 0.0;
    }

    return v[1];
}

ON_3dVector
ReparameterizationFunction::
EvaluateOriginalCurveDerivative(
    double t)
{
    double v[6];

    bool rc =
        curve.Evaluate(
            t,
            1,      // 一阶导数
            3,      // 三维
            v
        );

    if (!rc)
    {
        std::cout
            << "Error: Evaluate original curve derivative failed."
            << std::endl;

        return ON_3dVector(0.0, 0.0, 0.0);
    }

    // Evaluate 返回：
    //
    // v[0], v[1], v[2] = C(t)
    //
    // v[3], v[4], v[5] = C'(t)

    return ON_3dVector(
        v[3],
        v[4],
        v[5]
    );
}

std::vector<double>
ReparameterizationFunction::GetPhiKnotVector() const
{
    std::vector<double> knots;

    int knotCount = phi.KnotCount();

    knots.resize(knotCount);

    for (int i = 0; i < knotCount; ++i)
    {
        knots[i] = phi.Knot(i);
    }

    return knots;
}

BasisResult
ReparameterizationFunction::
EvaluatePhiBasisAndDerivatives(
    double u) const
{
    int degree =
        phi.Degree();

    std::vector<double> knots =
        GetPhiKnotVector();

    return computeBasisAndDerivatives_ON(
        degree,
        knots,
        u
    );
}

void TestPhiBasisConsistency(
    ReparameterizationFunction& rf,
    double u)
{
    BasisResult res =
        rf.EvaluatePhiBasisAndDerivatives(u);

    double phiManual = 0.0;

    for (int i = 0;
        i < rf.phi.CVCount();
        ++i)
    {
        phiManual +=
            res.N[i] *
            rf.beta[i];
    }

    double phiON =
        rf.EvaluatePhi(u);

    double basisSum = 0.0;

    for (double Ni : res.N)
    {
        basisSum += Ni;
    }

    double error =
        std::abs(
            phiManual -
            phiON
        );

    std::cout
        << "\n========================================"
        << std::endl;

    std::cout
        << "Test Phi Basis"
        << std::endl;

    std::cout
        << "u = "
        << u
        << std::endl;

    std::cout
        << "Basis sum = "
        << basisSum
        << std::endl;

    std::cout
        << "Phi manual = "
        << phiManual
        << std::endl;

    std::cout
        << "Phi ON      = "
        << phiON
        << std::endl;

    std::cout
        << "Error       = "
        << error
        << std::endl;

    std::cout
        << "Basis:"
        << std::endl;

    for (int i = 0;
        i < rf.phi.CVCount();
        ++i)
    {
        std::cout
            << "N["
            << i
            << "] = "
            << res.N[i]
            << std::endl;
    }
}

double ReparameterizationFunction::
EvaluatePhiDerivativeWrtEpsilon(
    double u,
    int epsilonIndex)
{
    //--------------------------------------
    // 1. 基本检查
    //--------------------------------------

    int cvCount = phi.CVCount();

    int epsCount =
        cvCount - 1;

    if (epsilonIndex < 0 ||
        epsilonIndex >= epsCount)
    {
        std::cout
            << "Invalid epsilon index."
            << std::endl;

        return 0.0;
    }

    //--------------------------------------
    // 2. 计算
    //
    // S = sum epsilon[k]^2
    //--------------------------------------

    double S = 0.0;

    for (int k = 0;
        k < epsCount;
        ++k)
    {
        S +=
            eps[k] * eps[k];
    }

    if (S <= 1e-14)
    {
        std::cout
            << "Error: epsilon square sum is zero."
            << std::endl;

        return 0.0;
    }

    //--------------------------------------
    // 3. 获取 phi 的 knot vector
    //--------------------------------------

    std::vector<double> knots(
        phi.KnotCount()
    );

    for (int i = 0;
        i < phi.KnotCount();
        ++i)
    {
        knots[i] =
            phi.Knot(i);
    }

    //--------------------------------------
    // 4. 计算所有 B-spline 基函数
    //--------------------------------------

    BasisResult basisResult =
        computeBasisAndDerivatives_ON(
            phi.Degree(),
            knots,
            u
        );

    //--------------------------------------
    // 5. 获取 epsilon_k
    //--------------------------------------

    double epsilonK =
        eps[epsilonIndex];

    //--------------------------------------
    // 6. 计算
    //
    // d phi / d epsilon_k
    //
    // = sum_i
    //
    // N_i(u)
    // *
    // d beta_i / d epsilon_k
    //--------------------------------------

    double derivative = 0.0;

    for (int i = 0;
        i < cvCount;
        ++i)
    {
        //----------------------------------
        // indicator:
        //
        // epsilon[k] 是否属于
        //
        // beta[i] 的累积和
        //
        // beta[i]
        // =
        // (eps[0]^2 + ... + eps[i-1]^2)
        // / S
        //----------------------------------

        double indicator = 0.0;

        if (epsilonIndex < i)
        {
            indicator = 1.0;
        }

        //----------------------------------
        // d beta[i] / d epsilon[k]
        //----------------------------------

        double dBeta =
            (2.0 * epsilonK / S)
            *
            (
                indicator
                - beta[i]
                );

        //----------------------------------
        // 累加
        //----------------------------------

        derivative +=
            basisResult.N[i]
            *
            dBeta;
    }

    return derivative;
}

void TestPhiEpsilonGradient(
    ReparameterizationFunction& rf,
    double u,
    int epsilonIndex
)
{
    double h = 1e-6;

    //--------------------------------------
    // 1. 保存原始 epsilon
    //--------------------------------------

    std::vector<double> originalEps =
        rf.eps;

    //--------------------------------------
    // 2. epsilon + h
    //--------------------------------------

    std::vector<double> epsPlus =
        originalEps;

    epsPlus[epsilonIndex] += h;

    bool rcPlus =
        rf.UpdatePhiFromEps(
            epsPlus
        );

    if (!rcPlus)
    {
        std::cout
            << "UpdatePhiFromEps plus failed."
            << std::endl;

        return;
    }

    double phiPlus =
        rf.EvaluatePhi(u);

    //--------------------------------------
    // 3. epsilon - h
    //--------------------------------------

    std::vector<double> epsMinus =
        originalEps;

    epsMinus[epsilonIndex] -= h;

    bool rcMinus =
        rf.UpdatePhiFromEps(
            epsMinus
        );

    if (!rcMinus)
    {
        std::cout
            << "UpdatePhiFromEps minus failed."
            << std::endl;

        return;
    }

    double phiMinus =
        rf.EvaluatePhi(u);

    //--------------------------------------
    // 4. 恢复原来的 phi
    //--------------------------------------

    rf.UpdatePhiFromEps(
        originalEps
    );

    //--------------------------------------
    // 5. 中心有限差分
    //--------------------------------------

    double numericalDerivative =
        (phiPlus - phiMinus)
        /
        (2.0 * h);

    //--------------------------------------
    // 6. 解析导数
    //--------------------------------------

    double analyticalDerivative =
        rf.EvaluatePhiDerivativeWrtEpsilon(
            u,
            epsilonIndex
        );

    //--------------------------------------
    // 7. 输出
    //--------------------------------------

    double error =
        std::abs(
            analyticalDerivative
            -
            numericalDerivative
        );

    std::cout
        << "\n========================================"
        << std::endl;

    std::cout
        << "Test dPhi / dEpsilon"
        << std::endl;

    std::cout
        << "u = "
        << u
        << std::endl;

    std::cout
        << "epsilon index = "
        << epsilonIndex
        << std::endl;

    std::cout
        << "Analytical derivative = "
        << analyticalDerivative
        << std::endl;

    std::cout
        << "Numerical derivative  = "
        << numericalDerivative
        << std::endl;

    std::cout
        << "Error                 = "
        << error
        << std::endl;
}

ON_3dVector
ReparameterizationFunction::
EvaluateReparameterizedCurveDerivativeWrtEpsilon(
    double u,
    int epsilonIndex)
{
    //--------------------------------------
    // 1. 计算 phi(u)
    //--------------------------------------

    double s =
        EvaluatePhi(u);

    //--------------------------------------
    // 2. 映射到原始曲线参数
    //
    // t = t0 + (t1 - t0) * phi(u)
    //--------------------------------------

    double t =
        t0
        +
        (t1 - t0)
        *
        s;

    //--------------------------------------
    // 3. 计算 C'(t)
    //--------------------------------------

    ON_3dVector dCdt =
        EvaluateOriginalCurveDerivative(
            t
        );

    //--------------------------------------
    // 4. 计算
    //
    // d phi / d epsilon[k]
    //--------------------------------------

    double dPhi_dEps =
        EvaluatePhiDerivativeWrtEpsilon(
            u,
            epsilonIndex
        );

    //--------------------------------------
    // 5. 链式法则
    //
    // dP / dEpsilon
    //
    // =
    //
    // C'(t)
    // *
    // (t1 - t0)
    // *
    // dPhi/dEpsilon
    //--------------------------------------

    double scale =
        (t1 - t0)
        *
        dPhi_dEps;

    return ON_3dVector(
        dCdt.x * scale,
        dCdt.y * scale,
        dCdt.z * scale
    );
}

void TestReparameterizedCurveGradient(
    ReparameterizationFunction& rf,
    double u,
    int epsilonIndex)
{
    double h = 1e-6;

    //--------------------------------------
    // 保存原始 epsilon
    //--------------------------------------

    std::vector<double> originalEps =
        rf.eps;

    //--------------------------------------
    // epsilon + h
    //--------------------------------------

    std::vector<double> epsPlus =
        originalEps;

    epsPlus[epsilonIndex] += h;

    bool rc =
        rf.UpdatePhiFromEps(
            epsPlus
        );

    if (!rc)
    {
        std::cout
            << "UpdatePhiFromEps (+h) failed."
            << std::endl;

        return;
    }

    ON_3dPoint P_plus =
        rf.EvaluateReparameterizedCurve(
            u
        );

    //--------------------------------------
    // epsilon - h
    //--------------------------------------

    std::vector<double> epsMinus =
        originalEps;

    epsMinus[epsilonIndex] -= h;

    rc =
        rf.UpdatePhiFromEps(
            epsMinus
        );

    if (!rc)
    {
        std::cout
            << "UpdatePhiFromEps (-h) failed."
            << std::endl;

        return;
    }

    ON_3dPoint P_minus =
        rf.EvaluateReparameterizedCurve(
            u
        );

    //--------------------------------------
    // 恢复原始状态
    //--------------------------------------

    rf.UpdatePhiFromEps(
        originalEps
    );

    //--------------------------------------
    // 中心有限差分
    //--------------------------------------

    ON_3dVector numericalDerivative(
        (P_plus.x - P_minus.x)
        / (2.0 * h),

        (P_plus.y - P_minus.y)
        / (2.0 * h),

        (P_plus.z - P_minus.z)
        / (2.0 * h)
    );

    //--------------------------------------
    // 解析梯度
    //--------------------------------------

    ON_3dVector analyticalDerivative =
        rf.EvaluateReparameterizedCurveDerivativeWrtEpsilon(
            u,
            epsilonIndex
        );

    //--------------------------------------
    // 误差
    //--------------------------------------

    ON_3dVector difference =
        analyticalDerivative
        -
        numericalDerivative;

    double error =
        difference.Length();

    //--------------------------------------
    // 输出
    //--------------------------------------

    std::cout
        << "\n========================================"
        << std::endl;

    std::cout
        << "Test dP / dEpsilon"
        << std::endl;

    std::cout
        << "u = "
        << u
        << std::endl;

    std::cout
        << "epsilon index = "
        << epsilonIndex
        << std::endl;

    std::cout
        << "Analytical derivative:"
        << std::endl;

    std::cout
        << "dx = "
        << analyticalDerivative.x
        << std::endl;

    std::cout
        << "dy = "
        << analyticalDerivative.y
        << std::endl;

    std::cout
        << "dz = "
        << analyticalDerivative.z
        << std::endl;

    std::cout
        << "Numerical derivative:"
        << std::endl;

    std::cout
        << "dx = "
        << numericalDerivative.x
        << std::endl;

    std::cout
        << "dy = "
        << numericalDerivative.y
        << std::endl;

    std::cout
        << "dz = "
        << numericalDerivative.z
        << std::endl;

    std::cout
        << "Vector error = "
        << error
        << std::endl;
}
/////////////////////////////////////阶段二///////////////////////////////////////
ON_3dPoint ReparameterizationFunction::EvaluateReparameterizedCurve(
    double u)
{
    // 1. phi(u)

    double s = EvaluatePhi(u);

    // 2. 映射到原始参数

    double t = t0 + (t1 - t0) * s;

    // 3. C(t)

    ON_3dPoint p;

    bool rc =
        curve.Evaluate(
            t,
            0,
            3,
            p);

    if (!rc)
    {
        std::cout
            << "Error: Evaluate phi failed at u = "
            << u
            << std::endl;
        return ON_3dPoint(0, 0, 0);
    }

    return p;
}

void SkinningOptimizer::Initialize()
{
    eps.clear();
    epsStartIndex.clear();

    totalVariableCount = 0;

    // 参数方向采样点数量
    sampleCount = 100;

    for (size_t i = 0; i < sections.size(); ++i)
    {
        // 第 i 条截面的第一个 epsilon
        // 在全局 eps 中的位置
        epsStartIndex.push_back(
            static_cast<int>(eps.size())
        );

        // 将该截面的 epsilon 加入全局 epsilon
        for (double e : sections[i].eps)
        {
            eps.push_back(e);
        }
    }

    totalVariableCount =
        static_cast<int>(eps.size());
}
void SkinningOptimizer::UpdateAllPhi()
{
    for (size_t i = 0; i < sections.size(); ++i)
    {
        int startIndex = epsStartIndex[i];

        int localCount =
            static_cast<int>(sections[i].eps.size());

        std::vector<double> localEps;

        localEps.reserve(localCount);

        for (int j = 0; j < localCount; ++j)
        {
            localEps.push_back(
                eps[startIndex + j]
            );
        }

        sections[i].UpdatePhiFromEps(localEps);
    }
}


double SkinningOptimizer::ComputeSkinFairnessEnergy()
{
    double energy = 0.0;

    int sectionCount =
        static_cast<int>(sections.size());

    for (int i = 1; i < sectionCount - 1; ++i)
    {
        for (int j = 0; j < sampleCount; ++j)
        {
            double u =
                static_cast<double>(j)
                / static_cast<double>(sampleCount - 1);

            ON_3dPoint P_prev =
                sections[i - 1]
                .EvaluateReparameterizedCurve(u);

            ON_3dPoint P_curr =
                sections[i]
                .EvaluateReparameterizedCurve(u);

            ON_3dPoint P_next =
                sections[i + 1]
                .EvaluateReparameterizedCurve(u);

            double dx =
                P_next.x
                - 2.0 * P_curr.x
                + P_prev.x;

            double dy =
                P_next.y
                - 2.0 * P_curr.y
                + P_prev.y;

            double dz =
                P_next.z
                - 2.0 * P_curr.z
                + P_prev.z;

            energy +=
                dx * dx +
                dy * dy +
                dz * dz;
        }
    }

    return energy;
}

double SkinningOptimizer::ComputeObjective()
{
    // 先根据当前全局 epsilon 更新所有 phi
    UpdateAllPhi();

    return ComputeSkinFairnessEnergy();

}

void SkinningOptimizer::ComputeObjectiveGradient(
    std::vector<double>& gradient)
{
    //--------------------------------------
    // 1. 确保所有截面的 phi
    //    与当前全局 eps 一致
    //--------------------------------------

    UpdateAllPhi();

    //--------------------------------------
    // 2. 初始化全局梯度
    //--------------------------------------

    gradient.assign(
        totalVariableCount,
        0.0
    );

    int sectionCount =
        static_cast<int>(sections.size());

    //--------------------------------------
    // 3. 遍历每个全局优化变量
    //--------------------------------------

    for (int r = 0;
        r < sectionCount;
        ++r)
    {
        //----------------------------------
        // 当前截面的局部变量数量
        //----------------------------------

        int localEpsCount =
            static_cast<int>(
                sections[r].eps.size()
                );

        //----------------------------------
        // 当前截面在全局 eps 中的起始位置
        //----------------------------------

        int startIndex =
            epsStartIndex[r];

        //----------------------------------
        // 遍历当前截面的每个 epsilon
        //----------------------------------

        for (int k = 0;
            k < localEpsCount;
            ++k)
        {
            double derivative = 0.0;

            //--------------------------------
            // 遍历所有二阶差分项
            //--------------------------------

            for (int i = 1;
                i < sectionCount - 1;
                ++i)
            {
                //--------------------------------
                // 判断 epsilon[r][k]
                // 是否影响 D_i
                //
                // D_i =
                // P_{i+1}
                // - 2 P_i
                // + P_{i-1}
                //--------------------------------

                double coefficient = 0.0;

                if (r == i - 1)
                {
                    coefficient = 1.0;
                }
                else if (r == i)
                {
                    coefficient = -2.0;
                }
                else if (r == i + 1)
                {
                    coefficient = 1.0;
                }
                else
                {
                    // 当前 epsilon 不影响这个 D_i
                    continue;
                }

                //--------------------------------
                // 遍历参数采样点
                //--------------------------------

                for (int j = 0;
                    j < sampleCount;
                    ++j)
                {
                    double u =
                        static_cast<double>(j)
                        /
                        static_cast<double>(
                            sampleCount - 1
                            );

                    //--------------------------------
                    // 计算 D_i
                    //--------------------------------

                    ON_3dPoint P_prev =
                        sections[i - 1]
                        .EvaluateReparameterizedCurve(
                            u
                        );

                    ON_3dPoint P_curr =
                        sections[i]
                        .EvaluateReparameterizedCurve(
                            u
                        );

                    ON_3dPoint P_next =
                        sections[i + 1]
                        .EvaluateReparameterizedCurve(
                            u
                        );

                    ON_3dVector D(
                        P_next.x
                        - 2.0 * P_curr.x
                        + P_prev.x,

                        P_next.y
                        - 2.0 * P_curr.y
                        + P_prev.y,

                        P_next.z
                        - 2.0 * P_curr.z
                        + P_prev.z
                    );

                    //--------------------------------
                    // 当前 epsilon 对
                    // P_r 的导数
                    //--------------------------------

                    ON_3dVector dP =
                        sections[r]
                        .EvaluateReparameterizedCurveDerivativeWrtEpsilon(
                            u,
                            k
                        );

                    //--------------------------------
                    // dD / d epsilon
                    //
                    // = coefficient * dP
                    //--------------------------------

                    ON_3dVector dD(
                        coefficient * dP.x,
                        coefficient * dP.y,
                        coefficient * dP.z
                    );

                    //--------------------------------
                    // d ||D||^2 / d epsilon
                    //
                    // = 2 D · dD
                    //--------------------------------

                    derivative +=
                        2.0
                        *
                        (
                            D.x * dD.x
                            +
                            D.y * dD.y
                            +
                            D.z * dD.z
                            );
                }
            }

            //--------------------------------
            // 写入对应的全局梯度
            //--------------------------------

            int globalIndex =
                startIndex + k;

            gradient[globalIndex] =
                derivative;
        }
    }
}

void TestSkinFairnessGradient(
    SkinningOptimizer& optimizer)
{
    const double h = 1e-6;

    //--------------------------------------
    // 保存原始全局 epsilon
    //--------------------------------------

    std::vector<double> originalEps =
        optimizer.eps;


    //--------------------------------------
    // 确保原始状态一致
    //--------------------------------------

    optimizer.eps =
        originalEps;

    optimizer.UpdateAllPhi();


    //--------------------------------------
    // 计算解析梯度
    //--------------------------------------

    std::vector<double> analyticalGradient;

    optimizer.ComputeObjectiveGradient(
        analyticalGradient
    );


    //--------------------------------------
    // 输出标题
    //--------------------------------------

    std::cout
        << "\n========================================"
        << std::endl;

    std::cout
        << "Test Skin Fairness Gradient"
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;

    std::cout
        << std::scientific
        << std::setprecision(15);

    std::cout
        << "Total variable count = "
        << optimizer.totalVariableCount
        << std::endl;

    std::cout
        << "Finite difference h  = "
        << h
        << std::endl;


    //--------------------------------------
    // 最大误差统计
    //--------------------------------------

    double maxAbsoluteError = 0.0;

    double maxRelativeError = 0.0;

    int maxErrorIndex = -1;


    //--------------------------------------
    // 逐个全局变量验证
    //--------------------------------------

    for (int k = 0;
        k < optimizer.totalVariableCount;
        ++k)
    {
        //----------------------------------
        // 1. epsilon + h
        //----------------------------------

        optimizer.eps =
            originalEps;

        optimizer.eps[k] += h;

        double energyPlus =
            optimizer.ComputeObjective();


        //----------------------------------
        // 2. epsilon - h
        //----------------------------------

        optimizer.eps =
            originalEps;

        optimizer.eps[k] -= h;

        double energyMinus =
            optimizer.ComputeObjective();


        //----------------------------------
        // 3. 中心有限差分梯度
        //----------------------------------

        double numericalDerivative =
            (energyPlus - energyMinus)
            /
            (2.0 * h);


        //----------------------------------
        // 4. 解析梯度
        //----------------------------------

        double analyticalDerivative =
            analyticalGradient[k];


        //----------------------------------
  // 5. 计算误差
  //----------------------------------

        double absoluteError =
            std::abs(
                analyticalDerivative
                -
                numericalDerivative
            );

        double denominator = 1.0;

        if (std::abs(analyticalDerivative) > denominator)
        {
            denominator =
                std::abs(analyticalDerivative);
        }

        if (std::abs(numericalDerivative) > denominator)
        {
            denominator =
                std::abs(numericalDerivative);
        }

        double relativeError =
            absoluteError / denominator;


        //----------------------------------
        // 输出
        //----------------------------------

        std::cout
            << "\n----------------------------------------"
            << std::endl;

        std::cout
            << "Global epsilon index = "
            << k
            << std::endl;

        std::cout
            << "Analytical derivative = "
            << analyticalDerivative
            << std::endl;

        std::cout
            << "Numerical derivative  = "
            << numericalDerivative
            << std::endl;

        std::cout
            << "Absolute error        = "
            << absoluteError
            << std::endl;

        std::cout
            << "Relative error        = "
            << relativeError
            << std::endl;


        //----------------------------------
        // 更新最大误差
        //----------------------------------

        if (absoluteError > maxAbsoluteError)
        {
            maxAbsoluteError =
                absoluteError;

            maxErrorIndex =
                k;
        }

        if (relativeError > maxRelativeError)
        {
            maxRelativeError =
                relativeError;
        }
    }


    //--------------------------------------
    // 恢复原始状态
    //--------------------------------------

    optimizer.eps =
        originalEps;

    optimizer.UpdateAllPhi();


    //--------------------------------------
    // 最终输出
    //--------------------------------------

    std::cout
        << "\n========================================"
        << std::endl;

    std::cout
        << "Gradient Test Summary"
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;

    std::cout
        << "Maximum absolute error = "
        << maxAbsoluteError
        << std::endl;

    std::cout
        << "Maximum relative error = "
        << maxRelativeError
        << std::endl;

    std::cout
        << "Maximum error index    = "
        << maxErrorIndex
        << std::endl;


    //--------------------------------------
    // 判断结果
    //--------------------------------------

    if (maxRelativeError < 1e-5)
    {
        std::cout
            << "\nPASS:"
            << std::endl;

        std::cout
            << "The analytical gradient of "
            << "E_skin matches the finite "
            << "difference gradient."
            << std::endl;
    }
    else
    {
        std::cout
            << "\nFAIL:"
            << std::endl;

        std::cout
            << "The analytical gradient does "
            << "not match the finite difference "
            << "gradient."
            << std::endl;
    }
}