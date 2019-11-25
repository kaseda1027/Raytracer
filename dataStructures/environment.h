#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "light.h"
#include "sphere.h"
#include <boost/tokenizer.hpp>
#include <Eigen/Dense>
#include <vector>
#include <string>

class Environment {
  public:
    Eigen::Vector3d eye;
    Eigen::Vector3d look;
    Eigen::Vector3d up;
    Eigen::Vector3d uCam;
    Eigen::Vector3d vCam;
    Eigen::Vector3d wCam;
    double focalLength;
    double minHor, minVer, maxHor, maxVer;
    long xRes, yRes;
    Eigen::Vector3d amb;
    std::vector<Light> lightSources;
    std::vector<Sphere> spheres;

    Environment(const std::string &driverFile);

  private:
    void processLine(const std::string &);
    void processEye();
    void processLook();
    void processUp();
    void processFocalLength();
    void processBounds();
    void processRes();
    void processAmbient();
    void processLight();
    void processSphere();
    void setupCamera();
    void sortSpheres();
    double getOneVal();

    boost::tokenizer<boost::char_separator<char>>::iterator lineIt;
    boost::tokenizer<boost::char_separator<char>>::iterator lineEnd;
};

double distance(Eigen::Vector3d, Eigen::Vector3d);

// Enumeration for types of lines we can read in
#define INVALID -1
#define COMMENT 0
#define EYE 1
#define LOOK 2
#define UP 3
#define FOC_LENGTH 4
#define BOUNDS 5
#define RES 6
#define AMBIENT 7
#define LIGHT 8
#define SPHERE 9

#endif
