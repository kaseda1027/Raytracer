#include "./material.h"
#include <boost/tokenizer.hpp>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <cstdlib>

using namespace boost;
using namespace Eigen;
using namespace std;

void processAmbient(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.ambient(0) = atof((*it++).c_str());
    mat.ambient(1) = atof((*it++).c_str());
    mat.ambient(2) = atof((*it++).c_str());
}

void processDiffuse(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.diffuse(0) = atof((*it++).c_str());
    mat.diffuse(1) = atof((*it++).c_str());
    mat.diffuse(2) = atof((*it++).c_str());
}

void processSpecular(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.specular(0) = atof((*it++).c_str());
    mat.specular(1) = atof((*it++).c_str());
    mat.specular(2) = atof((*it++).c_str());
    // For all models at the moment, Kr = Ks
    mat.reflective = mat.specular;
}

void processExponent(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.specularExponent = atof((*it++).c_str());
}

void processIllum(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.illuminationModel = atoi((*it++).c_str());
}

void processNewMat(vector<Material> &vecToExtend, tokenizer<char_separator<char>>::iterator it) {
    vecToExtend.emplace_back();
    vecToExtend.back().name = *it;
}

void processRefractiveIndex(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.refractiveIndex = atof((*it++).c_str());
}

void processTransparency(Material &mat, tokenizer<char_separator<char>>::iterator it) {
    mat.transparency(0) = atof((*it++).c_str());
    mat.transparency(1) = atof((*it++).c_str());
    mat.transparency(2) = atof((*it++).c_str());
}

void processMaterialLine(vector<Material> &vecToExtend, const string &line) {
    char_separator<char> sep(" \n\t\r");
    tokenizer<char_separator<char>> tokens(line, sep);
    auto lineIt = tokens.begin();
    const string type(*lineIt++);
    if(type == "Ka") processAmbient(vecToExtend.back(), lineIt);
    if(type == "Kd") processDiffuse(vecToExtend.back(), lineIt);
    if(type == "Ks") processSpecular(vecToExtend.back(), lineIt);
    if(type == "Ns") processExponent(vecToExtend.back(), lineIt);
    if(type == "Ni") processRefractiveIndex(vecToExtend.back(), lineIt);
    if(type == "Tr") processTransparency(vecToExtend.back(), lineIt);
    if(type == "illum") processIllum(vecToExtend.back(), lineIt);
    if(type == "newmtl") processNewMat(vecToExtend, lineIt);
}

void processMaterialFile(vector<Material> &vecToExtend, ifstream &file) {
    string line;
    while(getline(file, line)) {
        if(line.find_first_not_of(" \t\r") == string::npos) continue;
        processMaterialLine(vecToExtend, line);
    }
}

void materialFactory(vector<Material> &vecToExtend, const string &materialFile) {
    ifstream file(materialFile);
    if(!file) {
        throw ("Couldn't open material file (" + materialFile + ") for reading");
    }
    return processMaterialFile(vecToExtend, file);
}
