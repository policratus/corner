/*
 * Corner library header definitions
*/

#ifndef LIBCORNER_HPP_INCLUDED
#define LIBCORNER_HPP_INCLUDED

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
        explicit Corner(Size dimensions): maximumDimensions{dimensions} {}

        Point2f markerSize;

        Mat getMarker(string const& filePath);
        unsigned short int numberOfCameraDevices();
        tuple<string, path, float, float> commandLine(int const argumentCount, char** const argumentValues);
        void findMarker(Mat image, Mat &frame, Size2f measurements, Point2f &norms);
        void resizeToFit(Mat &image);
        RotatedRect boundingBox(Mat image);
        Size2f measure(Point2f* rotatedBox, Point2f markerNorms);
        void drawMeasurementsObject(Mat &image, Point2f markerNorms);
};


#endif
