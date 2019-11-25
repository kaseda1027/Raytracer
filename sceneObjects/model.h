#ifndef MODEL_H
#define MODEL_H

#include "sceneObject.h"
#include "../dataStructures/material.h"
#include "../dataStructures/ray.h"
#include "../dataStructures/face.h"
#include "transformation.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <Eigen/Dense>

class Model: public SceneObject {
    public:
        Model() = delete;
        Model(const Model &) = default;
        Model(const std::string &modelLine);
        virtual ~Model() = default;

        std::vector<Material> materials;
        void intersectRay(Ray &ray);
        void intersectRayWithEarlyTermination(Ray &);
        Ray getRefractionRay(Ray &);
        void transform(Transformation &transform);
        int numFaces = 0;

    private:
        double smoothingCutoff;
        int currentMaterial = -1;
        Eigen::Matrix<double, 4, Eigen::Dynamic> vertices;
        std::vector<std::vector<int>> vertexFaceRef;
        std::vector<Eigen::Vector3i> normals;
        std::vector<Face> faces;
        void buildFromWavefrontObjectFile(const std::string &fileName);
        void convertVectorsToMatrix(const std::vector<Eigen::Vector3d> &verts);
        void convertWavefrontObjectFileToVector(const std::string &fileName, std::vector<Eigen::Vector3d> &vertices);
        void processNewFace(const std::string &line);
        void processNewMaterials(const std::string &line);
        void processUseMaterial(const std::string &line);
        void faceIntersectRay(const Face &, Ray &);
        void calculateSurfaceNormals();
};

#endif
