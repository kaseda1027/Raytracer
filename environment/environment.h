#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../dataStructures/light.h"
#include "../sceneObjects/sceneObject.h"
#include <boost/tokenizer.hpp>
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <memory>

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
    std::vector<std::shared_ptr<SceneObject>> sceneObjects;
    int recursionLevel;
    int numFaces = 0;
    bool transparentShadows = false;

    Environment(const std::string &driverFile);
    Environment() = default;
    Environment(const Environment &) = default;
    Environment &operator=(const Environment &) = default;

  private:
    void processLine(const std::string &);
    void processLineByType(const std::string &);
    void processEye();
    void processLook();
    void processUp();
    void processFocalLength();
    void processBounds();
    void processRes();
    void processAmbient();
    void processLight();
    void processSphere();
    void processModel(const std::string &);
    void processRecursionLevel();
    void processTransparentShadows();
    void setupCamera();
    double getOneVal();

    boost::tokenizer<boost::char_separator<char>>::iterator lineIt;
    boost::tokenizer<boost::char_separator<char>>::iterator lineEnd;
};

double distance(Eigen::Vector3d, Eigen::Vector3d);

#endif
