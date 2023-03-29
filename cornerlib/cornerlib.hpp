/*
 * Corner library header definitions
*/

#ifndef CORNER_HPP_INCLUDED
#define CORNER_HPP_INCLUDED

#include <opencv2/core/mat.hpp>


using namespace std;

using namespace cv;


class Corner{
    public:
        Mat getMarker(string filePath);
        unsigned short int numberOfCameraDevices();
};

#endif
