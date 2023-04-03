/*
 * Corner library header definitions
*/

#ifndef CORNER_HPP_INCLUDED
#define CORNER_HPP_INCLUDED

#include <filesystem>

#include <opencv2/core/mat.hpp>


using namespace std;
using namespace filesystem;

using namespace cv;


class Corner{
    public:
        Mat getMarker(string filePath);
        unsigned short int numberOfCameraDevices();
        double resizeRespectingRatio(Size currentSize);
        tuple<string, path, float, float> commandLine(int const argumentCount, char** const argumentValues);
};

#endif
