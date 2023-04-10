/*
 * Corner library methods implementations
*/

#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "cornerlib.hpp"


using namespace std;
using namespace filesystem;

using namespace cv;


Mat Corner::getMarker(string const filePath){
    /*
     * Reads a fiducial marker image that will be used as reference
    */
    Mat image;

    if (!exists(filePath) || !haveImageReader(filePath)){
        cerr << endl << "Problems reading marker file in " + filePath << endl;
        exit(EXIT_FAILURE);
    }

    image = imread(filePath);

    resizeToFit(image);

    return image;
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
    if (currentSize.height > maximumDimensions.height)
        return (double)maximumDimensions.height / currentSize.height;

    if (currentSize.width > Corner::maximumDimensions.width)
        return (double)maximumDimensions.width / currentSize.width;

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


void Corner::features(Mat image, Mat &frame){
    const unsigned short int minimumMatches = 8;

    vector<KeyPoint> keypointsImage, keypointsFrame;
    Mat descriptorsImage, descriptorsFrame, homography;

    Ptr<SIFT> detector = SIFT::create();

    detector->detectAndCompute(image, noArray(), keypointsImage, descriptorsImage);
    detector->detectAndCompute(frame, noArray(), keypointsFrame, descriptorsFrame);

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);

    vector<vector<DMatch>> matches;
    vector<DMatch> goodMatches;
    vector<Point> matchesQuery, matchesTrain;

    matcher->knnMatch(descriptorsImage, descriptorsFrame, matches, 2);

    for (unsigned short int match = 0; match < matches.size(); match++){
        if (matches[match][0].distance < .7 * matches[match][1].distance){
            goodMatches.push_back(matches[match][0]);
            matchesQuery.push_back(keypointsImage[matches[match][0].queryIdx].pt);
            matchesTrain.push_back(keypointsFrame[matches[match][0].trainIdx].pt);
        }
    }

    if (goodMatches.size() >= minimumMatches){

        homography = findHomography(matchesQuery, matchesTrain, RANSAC);

        if (!homography.empty()){
            vector<Point2f> markerCorners(4);

            markerCorners[0] = Point2f(0, 0);
            markerCorners[1] = Point2f(image.cols, 0);
            markerCorners[2] = Point2f(image.cols, image.rows);
            markerCorners[3] = Point2f(0, image.rows);

            perspectiveTransform(markerCorners, markerCorners, homography);

            vector<Point> markerCornersLines;
            Mat(markerCorners).convertTo(markerCornersLines, CV_32S);

            polylines(frame, markerCornersLines, true, Scalar(72, 151, 224), 1, LINE_AA);
        }
    }
}


void Corner::resizeToFit(Mat &image){
    /*
     * Resizes an image (or frame) to fit the initialized dimensions
    */

    // The resize ratio, to later check if the frame must be resized or not.
    const double resizeRatio = resizeRespectingRatio(image.size());

    if (resizeRatio > .0)
        resize(image, image, Size(), resizeRatio, resizeRatio);
}
