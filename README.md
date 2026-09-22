# T-Spline Skinning with Parameter Consistency

## 1、Overview

This repository provides a C++ implementation of a T-spline skinning method with **parameter consistency** and **guiding curve interpolation**.

The method takes a sequence of NURBS cross-sections as input, reconstructs the cross-sections with consistent parameterization, and generates a T-spline surface through T-mesh construction and guiding curves. This work effectively alleviates isoparametric distortions, wiggles, and creases, while significantly improving surface fairness.

## Requirements

- C++
- [OpenNURBS](https://github.com/mcneel/opennurbs)
- [Eigen](https://eigen.tuxfamily.org/)

## Usage

1. Set the  input NURBS cross-section data in `main.cpp`.
2. Compile the project with OpenNURBS and Eigen.
3. Run the program to generate the parameter-consistent cross-sections and the corresponding T-spline surface.

The main implementation is contained in:

- `main.cpp` — main program and workflow
- `CGTS.cpp` — guiding curve and T-spline-related computation
- `Ohmethod.cpp` — T-mesh, basis functions, surface evaluation, and control-point computation

## Input

The program uses a sequence of NURBS cross-sections. The input data are specified in `main.cpp`.

For each cross-section, the user needs to provide:

- the control-point file;

- the curve order;

- the weights;

- the knot vector.

  For example:

  ```c++
  int dim = 3;
  int order = 4;
  
  auto ctrl1 = ReadControlPointsFromFile(
      "UGfigs/model1/POINTS1.DAT"
  );
  
  std::vector<double> weights1 = {
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
  };
  
  std::vector<double> knots1 = {
      0.0, 0.0, 0.0,
      0.1, 0.5, 0.7,
      1.0, 1.0, 1.0
  };
  
  ON_NurbsCurve curve1 =
      CreateNURBSCurve(
          dim, true, order,
          ctrl1, weights1, knots1
      );
  ```

  Additional cross-sections can be specified in the same way. The control-point files contain the coordinates of the NURBS control points.

## Output

The program exports the reconstructed curves and T-spline surfaces in **OBJ** files.

The generated OBJ files can be visualized using software such as Rhino, MeshLab, or CloudCompare.

## Acknowledgements

This project uses OpenNURBS for NURBS representation and Eigen for numerical computation.