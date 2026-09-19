#pragma once

#include <STEPControl_Reader.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>

#include <Geom_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <gp_Pnt.hxx>
#include <opennurbs.h>
#include <Eigen/Dense>
#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include "Ohmethod.h"
#include "Penaltyfunc.h"
static std::vector<double> expandKnots(
    const TColStd_Array1OfReal& knots,
    const TColStd_Array1OfInteger& mults);
static std::vector<double> computeGreville(
    const std::vector<double>& knot,
    int degree,
    int nCtrl);

void ExtractIsoVCurvesFromSurface(
    const Handle(Geom_BSplineSurface)& s,
    std::vector<ON_NurbsCurve>& curves,
    std::vector<double>& v_knots,
    std::vector<double>& v_params);
std::vector<TRow> buildTMeshFromCurves(
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_params);
void computeGlobalMeanCurvature_stp(
    int Nu, int Nv,
    const std::vector<TRow>& tmesh,
    const std::vector<ON_NurbsCurve>& curves,
    const std::vector<double>& v_knots,
    int degree_u, int degree_v,
    double u_min, double u_max,
    double v_min, double v_max);