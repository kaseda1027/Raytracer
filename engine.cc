#include "environment/environment.h"
#include "dataStructures/light.h"
#include "sceneObjects/sphere.h"
#include "dataStructures/ray.h"
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <chrono>

using namespace std;
using namespace Eigen;

string floatToIntColorString(const Vector3d &color) {
    int red =   max(0,min(255,static_cast<int>(round(color(0)*255))));
    int green = max(0,min(255,static_cast<int>(round(color(1)*255))));
    int blue =  max(0,min(255,static_cast<int>(round(color(2)*255))));
    return to_string(red) + ' ' + to_string(green) + ' ' + to_string(blue);
}

void intersectPixel(Ray &ray, const Environment &env) {
    for(auto &so: env.sceneObjects) {
         so->intersectRay(ray);
    }
}

Vector3d getShadowCoeff(Ray &ray, Environment &env) {
    Vector3d shadowCoeff = Vector3d(1.0, 1.0, 1.0);
    for(auto obj: env.sceneObjects) {
        obj->intersectRayWithEarlyTermination(ray);
        if(ray.intersectObject) {
            if(!env.transparentShadows || ray.material.transparency == Vector3d(0,0,0)) {
                return Vector3d(0,0,0);
            } else {
                 shadowCoeff = shadowCoeff.cwiseProduct(ray.material.transparency);
            }
        }
    }
    return shadowCoeff;
}

Vector3d pixelToColorVector(Ray &ray, Environment &env, int recursionLevel) {
    intersectPixel(ray, env);
    if(!ray.foundIntersect) {
        return Vector3d(0,0,0);
    }
    Material &mat = ray.material;
    Vector3d color = env.amb.cwiseProduct(mat.ambient);
    for(const Light &light: env.lightSources) {
        Vector3d dirToLight = light.pos - ray.intersect;
        dirToLight = dirToLight / dirToLight.norm();
        double intersectCosine = dirToLight.dot(ray.surfaceNormal);
        if(intersectCosine > 0) {
            Ray toLight;
            toLight.origin = ray.intersect+dirToLight*0.00000001;
            toLight.dir = dirToLight;
            toLight.foundIntersect = true;
            toLight.distanceToIntersect = (light.pos - toLight.origin).norm();
            toLight.intersectObject = nullptr;
            Vector3d shadowCoeff = getShadowCoeff(toLight, env);
            if(shadowCoeff != Vector3d(0,0,0)) {
                color += (mat.diffuse.cwiseProduct(light.color) * intersectCosine).cwiseProduct(shadowCoeff);
                Vector3d interToRay = (ray.origin - ray.intersect);
                interToRay = interToRay / interToRay.norm();
                Vector3d reflectionRay = 2*intersectCosine*ray.surfaceNormal - dirToLight;
                reflectionRay = reflectionRay / reflectionRay.norm();
                double reflectCosine = reflectionRay.dot(interToRay);
                if(reflectCosine > 0) {
                    color += (mat.specular.cwiseProduct(light.color)*pow(reflectCosine, mat.specularExponent)).cwiseProduct(shadowCoeff);
                }
            }
        }
    }
    if(recursionLevel > 0 && mat.illuminationModel >= 3) {
        Vector3d reflectionDir = -ray.dir;
        if(reflectionDir.dot(ray.surfaceNormal) >= 0.1 || dynamic_cast<Sphere *>(ray.intersectObject)) {
            reflectionDir = 2*reflectionDir.dot(ray.surfaceNormal)*ray.surfaceNormal - reflectionDir;
            reflectionDir = reflectionDir / reflectionDir.norm();
            Ray reflect;
            reflect.dir = reflectionDir;
            reflect.origin = ray.intersect;
            color += mat.reflective.cwiseProduct(pixelToColorVector(reflect, env, recursionLevel-1));
        }
    }
    if(recursionLevel > 0 && mat.illuminationModel >= 6 && ray.material.refractiveIndex > 0.0001 && ray.intersectObject) {
        try {
            Ray refractRay = ray.intersectObject->getRefractionRay(ray);
            color += mat.transparency.cwiseProduct(pixelToColorVector(refractRay, env, recursionLevel-1));
        } catch (string s) {}
    }
    return color;
}

string pixelToColor(double x, double y, Environment &env) {
    double distX = ((x/(env.xRes-1.0))*(env.maxHor - env.minHor)) + env.minHor;
    double distY = ((y/(env.yRes-1.0))*(env.minVer - env.maxVer)) + env.maxVer;
    Ray ray;
    ray.origin = env.eye + (-env.focalLength)*env.wCam + distX*env.uCam + distY*env.vCam;
    ray.dir = ray.origin - env.eye;
    ray.dir = ray.dir / ray.dir.norm();
    Vector3d color = pixelToColorVector(ray, env, env.recursionLevel);
    return floatToIntColorString(color);
}

int main(int argc, char **argv) {
    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " driverInput ppmOutput\n";
        return 1;
    }

    string driverFile(argv[1]);
    string outputFile(argv[2]);
    Environment env;

    auto startTime = chrono::steady_clock::now();

    try {
        env = Environment(driverFile);
    } catch(string s) {
        cerr << argv[0] << " Error: Failed to parse input file " << driverFile << '\n';
        cerr << s << '\n';
        return 1;
    }

    ofstream output(outputFile, ofstream::trunc);
    if(!output) {
        cerr << argv[0] << " Error: Failed to open output file " << outputFile << '\n';
        return 1;
    }

    auto curTime = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(curTime - startTime);
    double secElapsed = elapsed.count();
    cout << "Beginning scene rendering.\n"
         << "Scene resolution: " << env.xRes << " by " << env.yRes << "\n"
         << "Number of objects: " << env.sceneObjects.size() << "\n"
         << "Number of faces: " << env.numFaces << "\n"
         << "Number of lights: " << env.lightSources.size() << "\n"
         << "Recursion level: " << env.recursionLevel << "\n\n"
         << "Progress: 0.00%  Time Elapsed: " << secElapsed/1000.0 << " seconds";
    cout.flush();

    output << "P3\n";
    output << env.xRes << ' ' << env.yRes << " 255\n";

    int count = 0;
    int interval = max(2, static_cast<int>(env.xRes/100));

    for(int x = 0; x < env.xRes; x++) {
        string pixels[env.yRes];
        for(int y = 0;  y < env.yRes; y++)
            pixels[y] =  pixelToColor(y, x, env);
        for(int y = 0; y < env.yRes; y++)
            output << pixels[y] << ' ';
        output << '\n';
        if(++count == interval) {
            count = 0;
            curTime = chrono::steady_clock::now();
            elapsed = chrono::duration_cast<chrono::milliseconds>(curTime - startTime);
            secElapsed = elapsed.count();
            double fractionComplete = (x*1.0)/(env.xRes*1.0);
            double timeRemaining = 1.0/fractionComplete*secElapsed - secElapsed;
            cout << "\r" << string(100, ' ');
            cout << "\rProgress: " << fixed << setprecision(2) << fractionComplete*100.0 << "%  Time Elapsed: "
               << secElapsed/1000.0  << " seconds. Estimated time remaining: " << timeRemaining/1000.0 << " seconds";
            cout.flush();
        }
    }


    curTime = chrono::steady_clock::now();
    elapsed = chrono::duration_cast<chrono::milliseconds>(curTime - startTime);
    secElapsed = elapsed.count();
    cout << "\r" << string(100, ' ')
         << "\rProgress: 100.00%\n"
         << "Total Time Elapsed: " << secElapsed/1000.0 << " seconds\n"
         << "Raytracing complete! Output image should be saved in " << outputFile << ".\n";

    return 0;
}
