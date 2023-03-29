/*
 * Corner library methods implementations
*/

#include <cmath>
#include <iostream>
#include <filesystem>

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
    if (!exists(filePath)){
        cerr << "File " + filePath + " not found." << endl;
    } else if (!haveImageReader(filePath)){
        cerr << "File " + filePath + " isn't supported." << endl;
    }

    return imread(filePath);
}



unsigned short int Corner::numberOfCameraDevices(){
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


