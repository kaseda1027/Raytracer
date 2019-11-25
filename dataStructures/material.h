#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Dense>
#include <vector>
#include <string>

class Material {
  public:
    Eigen::Vector3d ambient = Eigen::Vector3d(0,0,0);
    Eigen::Vector3d diffuse = Eigen::Vector3d(0,0,0);
    Eigen::Vector3d specular = Eigen::Vector3d(0,0,0);
    Eigen::Vector3d reflective = Eigen::Vector3d(0,0,0);
    double specularExponent = 0;
    int illuminationModel = 6;
    double refractiveIndex = 0;
    Eigen::Vector3d transparency = Eigen::Vector3d(0,0,0);
    std::string name;
};

void materialFactory(std::vector<Material> &vecToExtend, const std::string &file);

#endif
