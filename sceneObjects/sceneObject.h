#ifndef SCENE_OBJ_H
#define SCENE_OBJ_H

#include "../dataStructures/ray.h"

class SceneObject {
  public:
    virtual void intersectRay(Ray &) = 0;
    virtual void intersectRayWithEarlyTermination(Ray &) = 0;
    virtual Ray getRefractionRay(Ray &) = 0;
    virtual ~SceneObject() = default;
  protected:
    Eigen::Vector3d getRefractionDir(Eigen::Vector3d toLight, Eigen::Vector3d normal, double etaFrom, double etaTo);
};

#endif
