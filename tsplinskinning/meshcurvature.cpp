#include "meshcurvature.h"

Mesh loadOBJ(const std::string& filename)
{
    Mesh mesh;
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Cannot open OBJ\n";
        return mesh;
    }

    int quadCount = 0;
    int triCount = 0;

    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            mesh.V.push_back(v);
        }
        else if (tag == "f") {
            std::vector<int> indices;
            std::string tmp;
            while (ss >> tmp) {
                indices.push_back(std::stoi(tmp) - 1);
            }

            if (indices.size() == 3) {
                mesh.F.push_back({ indices[0], indices[1], indices[2] });
                triCount++;
            }
            else if (indices.size() == 4) {
                // 拆成两个三角形
                mesh.F.push_back({ indices[0], indices[1], indices[2] });
                mesh.F.push_back({ indices[0], indices[2], indices[3] });
                quadCount++;
                triCount += 2;
            }
            else {
                std::cerr << "Warning: face with " << indices.size() << " vertices ignored\n";
            }
        }
    }

    std::cout << "OBJ loaded: " << quadCount << " quads split into " << triCount << " triangles.\n";
    return mesh;
}
void buildAdjacency(
    const Mesh& mesh,
    std::vector<std::vector<int>>& vertexNeighbors,//邻居顶点集合
    std::vector<std::vector<int>>& vertexFaces)//相邻面集合
{
    int n = mesh.V.size();
    vertexNeighbors.assign(n, {});
    vertexFaces.assign(n, {});

    for (int fi = 0; fi < mesh.F.size(); ++fi) {
        auto f = mesh.F[fi];

        for (int k = 0; k < 3; ++k) {
            int i = f[k];
            int j = f[(k + 1) % 3];

            vertexNeighbors[i].push_back(j);
            vertexNeighbors[j].push_back(i);

            vertexFaces[i].push_back(fi);
        }
    }
}
double triangleArea(const Vec3& a, const Vec3& b, const Vec3& c)
{
    return 0.5 * norm(cross(b - a, c - a));
}
double cotangent(const Vec3& a, const Vec3& b)
{
    double cosv = dot(a, b);
    double sinv = norm(cross(a, b));
    if (sinv < 1e-12) return 0.0;
    return cosv / sinv;
}
std::vector<double> computeMeanCurvature(const Mesh& mesh)
{
    int n = mesh.V.size();

    std::vector<std::vector<int>> vNeighbors, vFaces;
    buildAdjacency(mesh, vNeighbors, vFaces);

    std::vector<double> H(n, 0.0);

    // 每个顶点
    for (int i = 0; i < n; ++i)
    {
        Vec3 laplace = { 0,0,0 };
        double Ai = 0.0;

        // ---- 1. 计算面积 ----
        for (int fi : vFaces[i]) {
            auto f = mesh.F[fi];
            Vec3 a = mesh.V[f[0]];
            Vec3 b = mesh.V[f[1]];
            Vec3 c = mesh.V[f[2]];
            Ai += triangleArea(a, b, c) / 3.0;
        }

        if (Ai < 1e-12) continue;

        // ---- 2. 遍历邻边 ----
        for (int j : vNeighbors[i])
        {
            double cot_sum = 0.0;

            // 找包含(i,j)的两个面
            for (int fi : vFaces[i]) {
                auto f = mesh.F[fi];

                // 判断面是否包含边(i,j)
                if ((f[0] == i && f[1] == j) || (f[1] == i && f[0] == j) ||
                    (f[1] == i && f[2] == j) || (f[2] == i && f[1] == j) ||
                    (f[2] == i && f[0] == j) || (f[0] == i && f[2] == j))
                {
                    // 找第三个点k
                    int k = -1;
                    for (int t = 0; t < 3; ++t)
                        if (f[t] != i && f[t] != j)
                            k = f[t];

                    Vec3 vi = mesh.V[i];
                    Vec3 vj = mesh.V[j];
                    Vec3 vk = mesh.V[k];

                    Vec3 a = vi - vk;
                    Vec3 b = vj - vk;

                    cot_sum += cotangent(a, b);
                }
            }

            laplace = laplace + (mesh.V[j] - mesh.V[i]) * cot_sum;
        }

        laplace = laplace * (1.0 / (2.0 * Ai));

        H[i] = 0.5 * norm(laplace); // 无符号平均曲率
    }

    return H;
}
void computeGlobalMeanCurvature_OBJ(const Mesh& mesh)
{
    auto H = computeMeanCurvature(mesh);

    double Hmax = -1e100;
    double Hmin = 1e100;

    for (double h : H)
    {
        if (h < 1e-12) continue;

        if (h > Hmax) Hmax = h;
        if (h < Hmin) Hmin = h;
    }

    std::cout << "===== OBJ Mean Curvature =====\n";
    std::cout << "Max H = " << Hmax << std::endl;
    std::cout << "Min H = " << Hmin << std::endl;
    std::cout << "Difference = " << (Hmax - Hmin) << std::endl;
}

/*Mesh orimesh = loadOBJ("ug_tsurface.obj");

computeGlobalMeanCurvature_OBJ(orimesh);

Mesh rhinomesh = loadOBJ("rhinofan.obj");

computeGlobalMeanCurvature_OBJ(rhinomesh);
Mesh mesh = loadOBJ("ug_tsurface111_pro.obj");

computeGlobalMeanCurvature_OBJ(mesh);*/