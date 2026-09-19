#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include <opennurbs.h>
#include "Ohmethod.h"
#include "Penaltyfunc.h"
#include "paramatch.h"
#include "CGTS.h"
#include <vector>
#include <iostream>
//根据引导线弧长重塑初始网格
std::vector<TRow> buildTMesh_pro_pro(const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& para_v_firstcpt,
    const std::vector<ON_NurbsCurve>& allguidecurves,
    std::vector<std::vector<double>>& Adaptive_knots,
    int degree,
    const std::vector<std::vector<double>>& allGuideParams);
//调整曲面v参数减少褶皱
void redistributeVParameters(
    std::vector<TRow>& tmesh);