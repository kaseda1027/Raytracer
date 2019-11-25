#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <iomanip>

class Transformation {
    public:
       Transformation() = default;
       Transformation(const Transformation &) = default;
       // Driver line beginning with "model" and ending with file to be transformed
       Transformation(const std::string &driverLine);
       
       Eigen::Matrix4d getTransformationMatrix();

       // Axis of rotation
       double wx, wy, wz;
       double theta;
       double scale;
       // Translation
       double tx, ty, tz;
       double angleCutoff = 0.0;
       std::string file;

    private:
       void buildTransformationMatrix();
       Eigen::Matrix4d transformationMatrix;
};

#endif
