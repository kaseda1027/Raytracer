CXX=g++
CXXFLAGS=-O3 -Wall -std=c++11
TARGET=raytracer
SOURCE_FILES=environment/*.cc sceneObjects/*.cc dataStructures/*.cc engine.cc
HEADER_FILES=environment/*.h sceneObjects/*.h dataStructures/*.h
EIGEN_PATH=./Eigen # Change this line to the path of Eigen or place a symbolic link to Eigen to compile this program!

$(TARGET): $(SOURCE_FILES) $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -I $(EIGEN_PATH) -o $@ $(SOURCE_FILES)

clean:
	rm $(TARGET)
