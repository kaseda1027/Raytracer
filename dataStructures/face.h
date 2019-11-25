#include <Eigen/Dense>
#include <vector>
#include "./material.h"

class Face {
    public:
        Eigen::Vector3i vertexIndices;
        std::vector<Eigen::Vector3d> normals;
        Eigen::Vector3d trueNorm;
        int materialIndex;
};
