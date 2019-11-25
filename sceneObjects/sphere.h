#ifndef SPHERE_H
#define SPHERE_H

#include <Eigen/Dense>
#include "../dataStructures/material.h"
#include "../dataStructures/ray.h"
#include "sceneObject.h"

class Sphere: public SceneObject {
  public:
    Eigen::Vector3d center;
    double radius;
    Material material;
    void intersectRay(Ray &);
    void intersectRayWithEarlyTermination(Ray &);
    Ray getRefractionRay(Ray &);

    virtual ~Sphere() = default;
};

#endif
