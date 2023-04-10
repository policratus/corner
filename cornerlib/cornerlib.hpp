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
    private:
        const Size maximumDimensions;
        double resizeRespectingRatio(Size currentSize);

    public:
        Corner(Size dimensions): maximumDimensions{dimensions} {}

        Mat getMarker(string const filePath);
        unsigned short int numberOfCameraDevices();
        tuple<string, path, float, float> commandLine(int const argumentCount, char** const argumentValues);
        void features(Mat image, Mat &frame);
        void resizeToFit(Mat &image);
};

#endif
