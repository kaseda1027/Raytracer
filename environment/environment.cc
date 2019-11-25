#include <Eigen/Dense>
#include "environment.h"
#include "../sceneObjects/sphere.h"
#include "../sceneObjects/model.h"
#include "../dataStructures/material.h"
#include "../dataStructures/light.h"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace boost;
using namespace Eigen;

Environment::Environment(const string &driverFile) {
    ifstream file(driverFile);
    if(!file) {
        throw string("Couldn't open driver files");
    }
    string line;
    while(getline(file, line)) {
        if(!line.empty())
          processLine(line);
    }
    setupCamera();
}

void Environment::processLine(const string &line) {
    char_separator<char> sep(" \n\t\r");
    tokenizer<char_separator<char>> tokens((line), sep);
    lineIt = tokens.begin();
    lineEnd = tokens.end();
    try {
        processLineByType(line);
    } catch(string s) {
        throw string("Incomplete line in driver file:\n" + line + '\n' + s + '\n');
    }
}

void Environment::processTransparentShadows() {
    transparentShadows = getOneVal() == 1.0;   
}

void Environment::processLineByType(const string &line) {
    string type = *lineIt;
    if(type == "eye")     
          processEye();
    else if(type == "look")    
          processLook();        
    else if(type == "up")      
          processUp();          
    else if(type == "d")       
          processFocalLength(); 
    else if(type == "bounds")  
          processBounds();      
    else if(type == "res")     
          processRes();         
    else if(type == "ambient") 
          processAmbient();     
    else if(type == "light")   
          processLight();       
    else if(type == "sphere")  
          processSphere();      
    else if(type == "model")
          processModel(line);
    else if(type == "recursionlevel")
          processRecursionLevel();
    else if(type == "transparentShadows")
          processTransparentShadows();
    else if(type[0] == '#')    
          ; // Ignore comments, but they aren't invalid
    else
          throw string("Driver file contains invalid line\n");
}

double Environment::getOneVal() {
    if(++lineIt == lineEnd) {
        throw string("Ran out of input while parsing line\n");
    }
    return atof((*lineIt).c_str());
}

void Environment::processEye() {
    eye(0) = getOneVal();
    eye(1) = getOneVal();
    eye(2) = getOneVal();
}

void Environment::processLook() {
    look(0) = getOneVal();
    look(1) = getOneVal();
    look(2) = getOneVal();
}

void Environment::processUp() {
    up(0) = getOneVal();
    up(1) = getOneVal();
    up(2) = getOneVal();
}

void Environment::processFocalLength() {
    focalLength = getOneVal();
    if(focalLength < 0)
        focalLength = -focalLength;
}

void Environment::processBounds() {
    minHor = getOneVal();
    maxHor = getOneVal();
    minVer = getOneVal();
    maxVer = getOneVal();
}

void Environment::processRes() {
     xRes = getOneVal();
     yRes = getOneVal();
}

void Environment::processAmbient() {
    amb(0) =  getOneVal();
    amb(1) =  getOneVal();
    amb(2) =  getOneVal();
}

void Environment::processLight() {
    // Everything done on vector to avoid copying large amounts of data
    lightSources.emplace_back();
    Light &light = lightSources.back();
    light.pos(0) = getOneVal();
    light.pos(1) = getOneVal();
    light.pos(2) = getOneVal();
    light.atInfinity = getOneVal() == 0.0;
    light.color(0) = getOneVal();
    light.color(1) = getOneVal();
    light.color(2) = getOneVal();
}

void Environment::processSphere() {
    Sphere *sphere = new Sphere;
    sphere->center(0) = getOneVal();
    sphere->center(1) = getOneVal();
    sphere->center(2) = getOneVal();
    sphere->radius = getOneVal();
    sphere->material.ambient(0) = getOneVal();
    sphere->material.ambient(1) = getOneVal();
    sphere->material.ambient(2) = getOneVal();
    sphere->material.diffuse(0) = getOneVal();
    sphere->material.diffuse(1) = getOneVal();
    sphere->material.diffuse(2) = getOneVal();
    sphere->material.specular(0) = getOneVal();
    sphere->material.specular(1) = getOneVal();
    sphere->material.specular(2) = getOneVal();
    sphere->material.reflective(0) = getOneVal();
    sphere->material.reflective(1) = getOneVal();
    sphere->material.reflective(2) = getOneVal();
    sphere->material.refractiveIndex = getOneVal();
    sphere->material.transparency = Vector3d(1,1,1) - sphere->material.reflective;
    sphere->material.specularExponent = 16;
    sphere->material.illuminationModel = 6;
    sceneObjects.emplace_back(sphere);
}

void Environment::processModel(const string &line) {
    Model *model = new Model(line);
    numFaces += model->numFaces;
    sceneObjects.emplace_back(model);
}

void Environment::processRecursionLevel() {
    recursionLevel = getOneVal();
}

void Environment::setupCamera() {
    wCam = eye - look;
    wCam = wCam / wCam.norm();
    uCam = up.cross(wCam);
    uCam = uCam / uCam.norm();
    vCam = wCam.cross(uCam);
    vCam = vCam / vCam.norm();
    if(wCam == (up / up.norm())) {
        throw string("Camera points in same direction as Up\n");
    }
}

double distance(const Vector3d v1, const Vector3d v2) {
    return (v2-v1).norm();
};
