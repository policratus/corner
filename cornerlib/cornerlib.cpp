/*
 * Corner library methods implementations
*/

#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "cornerlib.hpp"


using namespace std;
using namespace filesystem;

using namespace cv;


Mat Corner::getMarker(string filePath){
    /*
     * Reads a fiducial marker image that will be used as reference
    */
    if (!exists(filePath) || !haveImageReader(filePath)){
        cerr << endl << "Problems reading marker file in " + filePath << endl;
        exit(EXIT_FAILURE);
    }

    return imread(filePath);
}


unsigned short int Corner::numberOfCameraDevices(){
    /*
     * Returns the number of cameras devices found
    */
    VideoCapture devices;
    unsigned short int numberOfDevices = 0;

    cout << "Scanning for camera devices. Please wait." << endl;

    for (unsigned short int device = 0; device < 5; ++device){
        devices.open(device);

        if (devices.isOpened()) ++numberOfDevices;

        devices.release();
    }

    return numberOfDevices;
}


double Corner::resizeRespectingRatio(Size currentSize){
    /*
     * Calculates the ratio of the current image or frame to transform its
     * dimensions to be smaller than the maximum accepted, otherwise
     * returns zero (meaning no changes necessary), always keeping
     * the image ratio.
    */
    Size const videoMaximumDimensions(1280, 720);

    if (currentSize.height > videoMaximumDimensions.height)
        return (double)videoMaximumDimensions.height / currentSize.height;

    if (currentSize.width > videoMaximumDimensions.width)
        return (double)videoMaximumDimensions.width / currentSize.width;

    return 0.;
}


tuple<string, path, float, float> Corner::commandLine(int const argumentCount, char** const argumentValues){
    // Command line options
    const String keys =
        "{help h usage ?||Help and usage for Corner}"
        "{@marker|<none>|Path where the image marker are stored}"
        "{@video|<none>|Path to the video file, streaming URI or webcam}"
        "{height|5.0|Height of the marker (in cm.)}"
        "{width|5.0|Width of the marker (in cm.)}"
        ;

    CommandLineParser parser(argumentCount, argumentValues, keys);
    parser.about("📏 Corner virtual rulers: measure objects with precision using images.");

    if (parser.has("help")) parser.printMessage();

    // If all mandatory command line options were filled
    if (
        !parser.has("@marker") || !parser.has("@video") || !parser.check()
    ){
        parser.printErrors();
        parser.printMessage();
    }

    return make_tuple(
        parser.get<String>("@video"),
        path(parser.get<String>("@marker")),
        parser.get<float>("height"),
        parser.get<float>("width")
    );
}
