#include "model.h"
#include "transformation.h"
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <cstdlib>
#include <cmath>

using namespace Eigen;
using namespace boost;
using namespace std;

int firstNonWhitespaceChar(const string &line) {
    return line.find_first_not_of(" \n\r\t");
}

bool isVertex(const string &line) {
    int start = firstNonWhitespaceChar(line);
    return (line.size()-start) >= 2
        && line.substr(start, 2) == "v ";
}

Vector3d convertLineToVertex(const string &line) {
    char_separator<char> sep(" \n\t\rv");
    tokenizer<char_separator<char> > tokens(line, sep);
    auto tokIt = tokens.begin();
    Vector3d vertex;
    vertex(0) = atof((*tokIt).c_str());
    vertex(1) = atof((*++tokIt).c_str());
    vertex(2) = atof((*++tokIt).c_str());
    return vertex;
}

bool isFace(const string &line) {
    int start = firstNonWhitespaceChar(line);
    return (line.size() - start) >= 1
        && line[start] == 'f';
}

int getFirstInt(const string &line) {
    return atof(line.substr(0, line.find_first_not_of("1234567890")).c_str());
}

void Model::processNewFace(const string &line) {
    char_separator<char> sep(" \n\t\r");
    tokenizer<char_separator<char> > tokens(line, sep);
    auto tokIt = tokens.begin();
    ++tokIt;
    Face face;
    Vector3i faceVertices;
    faceVertices(0) = getFirstInt(*(tokIt++))-1;
    faceVertices(1) = getFirstInt(*(tokIt++))-1;
    faceVertices(2) = getFirstInt(*(tokIt++))-1;
    face.vertexIndices = faceVertices;
    face.materialIndex = currentMaterial;
    faces.push_back(face);
    vertexFaceRef[faceVertices(0)].push_back(faces.size()-1);
    vertexFaceRef[faceVertices(1)].push_back(faces.size()-1);
    vertexFaceRef[faceVertices(2)].push_back(faces.size()-1);
}

bool isNewMaterial(const string &line) {
    int start = line.find_first_not_of(" \n\r\t");
    return line.length() - start > 6 &&
           line.substr(start, 6) == "mtllib";
}

void Model::processNewMaterials(const string &line) {
    const string file = line.substr(7);
    materialFactory(materials, file);
}

bool isUseMaterial(const string &line) {
    int start = line.find_first_not_of(" \n\r\t");
    return line.length() - start > 6 &&
           line.substr(start, 6) == "usemtl";
}

void Model::processUseMaterial(const string &line) {
    string materialName = line.substr(7); // 6 for 'usemtl' + 1 for space, start at 8
    int firstChar = materialName.find_first_not_of(" \n\r\t");
    int lastChar = materialName.find_last_not_of(" \n\r\t");
    materialName = materialName.substr(firstChar, lastChar + 1);
    for(size_t i = 0; i < materials.size(); i++) {
        if(materials[i].name == materialName) {
            currentMaterial = i;
            break;
        }
    }
}

void Model::convertWavefrontObjectFileToVector(const string &fileName, vector<Vector3d> &vertices) {
    ifstream file(fileName);
    for(string line; getline(file, line); ) {
        if(line.find_first_not_of(" \t\r") == string::npos) continue;
        if(isVertex(line)) {
            vertices.push_back(convertLineToVertex(line));
            vertexFaceRef.emplace_back();
            vertexFaceRef.back().clear();
        } else if(isFace(line)) {
            processNewFace(line);
            ++numFaces;
        } else if(isNewMaterial(line)) {
            processNewMaterials(line);
        } else if(isUseMaterial(line)) {
            processUseMaterial(line);
        }
    }
}

void Model::convertVectorsToMatrix(const vector<Vector3d> &verts) {
    vertices = Matrix<double, 4, Dynamic>(4, verts.size());
    for(size_t i = 0; i < verts.size(); i++) {
        vertices(0, i) = (verts[i])(0);
        vertices(1, i) = (verts[i])(1);
        vertices(2, i) = (verts[i])(2);
        vertices(3, i) = 1;
    }
}

void Model::buildFromWavefrontObjectFile(const string &fileName) {
    vector<Vector3d> verts;
    // There will probably be a ton of vertices, so pass by reference
    convertWavefrontObjectFileToVector(fileName, verts);
    // Size of matrix must be known beforehand, so process as vector first
    convertVectorsToMatrix(verts);
}

Model::Model(const string &line) {
    Transformation transformation(line);
    smoothingCutoff = transformation.angleCutoff;
    buildFromWavefrontObjectFile(transformation.file);
    transform(transformation);
    calculateSurfaceNormals();
}

void Model::transform(Transformation &transform) {
    this->vertices = transform.getTransformationMatrix() * this->vertices;
} 

void Model::calculateSurfaceNormals() {
    for(Face &face: faces) {
        int vert1 = face.vertexIndices(0);
        int vert2 = face.vertexIndices(1);
        int vert3 = face.vertexIndices(2);
        Vector3d vertex1(vertices(0, vert1), vertices(1, vert1), vertices(2, vert1));
        Vector3d vertex2(vertices(0, vert2), vertices(1, vert2), vertices(2, vert2));
        Vector3d vertex3(vertices(0, vert3), vertices(1, vert3), vertices(2, vert3));
        Vector3d surfaceNorm = (vertex1-vertex2).cross(vertex1-vertex3);
        face.trueNorm = surfaceNorm / surfaceNorm.norm();
        if(smoothingCutoff < 0.001) {
            face.normals.push_back(face.trueNorm);
            face.normals.push_back(face.trueNorm);
            face.normals.push_back(face.trueNorm);
        }
    }
    if(smoothingCutoff > 0.001) {
        for(Face &face: faces) {
            for(int i = 0; i < 3; i++) {
                Vector3d cumulativeNormal(0,0,0);
                for(const int faceIndex: vertexFaceRef[face.vertexIndices(i)]) {
                    Face &other = faces[faceIndex];
                    double cosine = max(-1.0, min(1.0, (other.trueNorm.dot(face.trueNorm))));
                    double angle = acos(cosine);
                    if(abs(angle) <= smoothingCutoff) {
                        cumulativeNormal += other.trueNorm;
                    }
                }
                face.normals.emplace_back(cumulativeNormal / cumulativeNormal.norm());
            }
        }
    }
}

void Model::faceIntersectRay(const Face &face, Ray &ray) {
    Matrix3d intersectMatrix;
    int vert1 = face.vertexIndices(0);
    int vert2 = face.vertexIndices(1);
    int vert3 = face.vertexIndices(2);
    Vector3d vertex1(vertices(0, vert1), vertices(1, vert1), vertices(2, vert1));
    Vector3d vertex2(vertices(0, vert2), vertices(1, vert2), vertices(2, vert2));
    Vector3d vertex3(vertices(0, vert3), vertices(1, vert3), vertices(2, vert3));
    Vector3d col1 = vertex1-vertex2; 
    Vector3d col2 = vertex1-vertex3; 
    Vector3d col3 = ray.dir;
    intersectMatrix << col1(0), col2(0), col3(0),
                       col1(1), col2(1), col3(1),
                       col1(2), col2(2), col3(2);
    Matrix3d inverse = intersectMatrix.inverse();
    Vector3d systemVec = vertex1 - ray.origin;
    Vector3d solution = inverse * systemVec;
    double beta  = solution(0);
    double gamma = solution(1);
    double distance = solution(2);
    if(distance > 0 && (!ray.foundIntersect || (distance-0.00001) < ray.distanceToIntersect)
      && gamma > 0 && beta > 0 && gamma + beta < 1) {
        ray.intersect = ray.origin + ray.dir*(distance - 0.00001);
        ray.distanceToIntersect = distance;
        Vector3d normal1 = face.normals[0];
        Vector3d normal2 = face.normals[1];
        Vector3d normal3 = face.normals[2];
        ray.surfaceNormal = normal1*(1-beta-gamma) + normal2*beta + normal3*gamma;
        ray.surfaceNormal = ray.surfaceNormal / ray.surfaceNormal.norm();
        if(ray.dir.dot(ray.surfaceNormal) > 0)
            ray.surfaceNormal = -ray.surfaceNormal;
        ray.material = materials[face.materialIndex];
        ray.foundIntersect = true;
        ray.intersectObject = this;
    }
}

void Model::intersectRay(Ray &ray) {
    for(const Face &face : faces) {
        faceIntersectRay(face, ray);
    }
}

void Model::intersectRayWithEarlyTermination(Ray &ray) {
    for(const Face &face : faces) {
        faceIntersectRay(face, ray);
        if(ray.intersectObject) {
            return;
        }
    }
}

Ray Model::getRefractionRay(Ray &ray) {
    Vector3d refractionDir = getRefractionDir(-ray.dir, ray.surfaceNormal, 1.0, ray.material.refractiveIndex);
    Ray refract;
    refract.dir = refractionDir;
    refract.origin = ray.intersect + ray.dir*0.0001;
    intersectRay(refract);
    refractionDir = getRefractionDir(-refract.dir, refract.surfaceNormal, ray.material.refractiveIndex, 1.0);
    Ray exit;
    exit.dir = refractionDir;
    exit.origin = refract.intersect + refract.dir*0.001;
    return exit;
}
