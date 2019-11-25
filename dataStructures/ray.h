#ifndef RAY_H
#define RAY_H

#include <Eigen/Dense>
#include <vector>
#include "material.h"
#include "light.h"

// Using forward declaration to prevent circular dependency
class SceneObject;
class Face;

class Ray {
  public:
    Eigen::Vector3d dir;
    Eigen::Vector3d origin;

    Eigen::Vector3d intersect;
    bool foundIntersect = false;
    Eigen::Vector3d surfaceNormal;
    double distanceToIntersect;
    Material material;
    SceneObject *intersectObject;
};

#endif
