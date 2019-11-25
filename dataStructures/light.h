#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Dense>

class Light {
  public:
    Eigen::Vector3d pos;
    bool atInfinity;
    Eigen::Vector3d color; // r, g, b
};

#endif
