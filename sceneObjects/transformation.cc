
#include "transformation.h"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cmath>

#define PI 3.1415926535897932384

using namespace std;
using namespace boost;
using namespace Eigen;

Transformation::Transformation(const string &driverLine) {
    char_separator<char> sep(" \n\t\r");
    tokenizer<char_separator<char> > tokens(driverLine, sep);
    auto tokIt = tokens.begin();
    if(*tokIt != "model")
        throw "Transformation: driverLine is not a model\n" + driverLine + '\n';
    wx = atof((*++tokIt).c_str());
    wy = atof((*++tokIt).c_str());
    wz = atof((*++tokIt).c_str());
    theta = atof((*++tokIt).c_str());
    scale = atof((*++tokIt).c_str());
    tx = atof((*++tokIt).c_str());
    ty = atof((*++tokIt).c_str());
    tz = atof((*++tokIt).c_str());
    angleCutoff = atof((*++tokIt).c_str());
    angleCutoff = angleCutoff*PI/180;
    file = *(++tokIt);
    buildTransformationMatrix();
}

Matrix4d getAxisRotationMatrix(double wx, double wy, double wz) {
    Vector3d zAxis(wx, wy, wz);
    zAxis = zAxis/zAxis.norm();
    Vector3d nonParallelVector(zAxis);
    if(nonParallelVector(0) == 1)
        nonParallelVector(1) = 1;
    else
        nonParallelVector(0) = 1;
    Vector3d xAxis = zAxis.cross(nonParallelVector);
    xAxis = xAxis/xAxis.norm();
    Vector3d yAxis = zAxis.cross(xAxis);
    Matrix4d rotationMatrix;
    rotationMatrix << xAxis(0), xAxis(1), xAxis(2), 0,
                      yAxis(0), yAxis(1), yAxis(2), 0,
                      zAxis(0), zAxis(1), zAxis(2), 0,
                      0, 0, 0, 1;
    return rotationMatrix;
}

Matrix4d getAngleRotationMatrix(double theta) {
    double radians = theta*PI/180;
    Matrix4d rotationMatrix;
    double acos = cos(radians);
    double asin = sin(radians);
    rotationMatrix << acos, -asin, 0, 0,
                      asin, acos, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1;
    return rotationMatrix;
}

Matrix4d getAxisAngleRotationMatrix(double wx, double wy, double wz, double theta) {
    Matrix4d rotateAxisToZ = getAxisRotationMatrix(wx, wy, wz);
    Matrix4d rotateAboutZ = getAngleRotationMatrix(theta);
    Matrix4d rotateAxisToZTranspose = rotateAxisToZ.transpose();
    Matrix4d AxisAngleRotationMatrix = rotateAxisToZTranspose * rotateAboutZ * rotateAxisToZ;
    return AxisAngleRotationMatrix;
}

Matrix4d getUniformScalingMatrix(double scale) {
    Matrix4d scaling;
    scaling << scale, 0, 0, 0,
               0, scale, 0, 0,
               0, 0, scale, 0,
               0, 0, 0, 1;
    return scaling;
}

Matrix4d getTranslationMatrix(double tx, double ty, double tz) {
    Matrix4d translate;
    translate << 1, 0, 0, tx,
                 0, 1, 0, ty,
                 0, 0, 1, tz,
                 0, 0, 0, 1;
    return translate;
}

void Transformation::buildTransformationMatrix() {
    Matrix4d rotation    = getAxisAngleRotationMatrix(wx, wy, wz, theta);
    Matrix4d scaling     = getUniformScalingMatrix(scale);
    Matrix4d translate   = getTranslationMatrix(tx, ty, tz);
    transformationMatrix = translate * scaling * rotation;
}

Matrix4d Transformation::getTransformationMatrix() {
    return transformationMatrix;
}

