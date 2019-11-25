#include "sceneObject.h"
#include <cmath>
#include <Eigen/Dense>
#include <string>

using namespace std;
using namespace Eigen;

Vector3d SceneObject::getRefractionDir(Vector3d rayDir, Vector3d normal, double etaFrom, double etaTo) {
    double refracIndexRatio = etaFrom/etaTo;
    double dotProd = rayDir.dot(normal);
    double radicalSqrd = refracIndexRatio*refracIndexRatio*(dotProd*dotProd-1)+1;
    if(radicalSqrd < 0.0001) {
         throw string("Refraction is not feasible");
    }
    double normCoeff = (refracIndexRatio * dotProd) - sqrt(radicalSqrd);
    Vector3d refractDir = (-refracIndexRatio)*rayDir + normCoeff*normal;
    return refractDir;
}
