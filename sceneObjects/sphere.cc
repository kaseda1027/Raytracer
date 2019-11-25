#include "sphere.h"
#include "../dataStructures/ray.h"
#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

void Sphere::intersectRay(Ray &ray) {
    Vector3d origToCent = center - ray.origin;
    double project = (origToCent).dot(ray.dir);
    double distToCentSqr = origToCent.dot(origToCent);
    double disc = radius*radius - (distToCentSqr - project*project);
    if(disc < 0.0001) return;
    double distFromProj = sqrt(disc);
    double distFromOrig = project - distFromProj;
    if(distFromOrig > 0 && (!ray.foundIntersect || (distFromOrig-0.001) < ray.distanceToIntersect)) {
        ray.distanceToIntersect = distFromOrig;
        ray.foundIntersect = true;
        ray.intersect = ray.origin + ray.distanceToIntersect*ray.dir;
        ray.surfaceNormal = ray.intersect - center;
        ray.surfaceNormal = ray.surfaceNormal / ray.surfaceNormal.norm();
        ray.material = material;
        ray.intersectObject = this;
    }
}

void Sphere::intersectRayWithEarlyTermination(Ray &ray) {
    intersectRay(ray);
}

Ray Sphere::getRefractionRay(Ray &ray) {
    Vector3d refractionDir = getRefractionDir(-ray.dir, ray.surfaceNormal, 1.0, ray.material.refractiveIndex);
    Vector3d exitPt = ray.intersect + 2*refractionDir.dot(center-ray.intersect)*refractionDir;
    Vector3d exitNorm = (this->center - exitPt);
    exitNorm = exitNorm / exitNorm.norm();
    Vector3d exitDir = getRefractionDir(-refractionDir, exitNorm, ray.material.refractiveIndex, 1.0);
    Ray exitRay;
    exitRay.dir = refractionDir;
    exitRay.dir = exitDir;
    exitRay.origin = exitPt+0.001*exitDir;
    return exitRay;
}
