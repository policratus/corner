/*
 * Corner library methods implementations
*/

#include <iostream>

#include <opencv2/core.hpp>
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
    if (currentSize.height > Corner::maximumDimensions.height)
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


void Corner::findMarker(Mat image, Mat &frame, Size2f measurements, Point2f &norms){
    /*
     * Finds the marker (if present in the image or frame) and segments it, applying
     * any perpective transformation needed.
    */
    const unsigned short int minimumMatches = 8;
    unsigned short int goodMatches = 0;

    vector<KeyPoint> keypointsImage, keypointsFrame;
    Mat descriptorsImage, descriptorsFrame, homography;

    Ptr<SIFT> detector = SIFT::create();

    detector->detectAndCompute(image, noArray(), keypointsImage, descriptorsImage);
    detector->detectAndCompute(frame, noArray(), keypointsFrame, descriptorsFrame);

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);

    vector<vector<DMatch>> matches;
    vector<Point> matchesQuery, matchesTrain;

    matcher->knnMatch(descriptorsImage, descriptorsFrame, matches, 2);

    for (unsigned short int match = 0; match < matches.size(); match++){
        if (matches[match][0].distance < .6 * matches[match][1].distance){
            goodMatches++;

            matchesQuery.push_back(keypointsImage[matches[match][0].queryIdx].pt);
            matchesTrain.push_back(keypointsFrame[matches[match][0].trainIdx].pt);
        }
    }

    if (goodMatches >= minimumMatches){

        homography = findHomography(matchesQuery, matchesTrain, RHO);

        if (!homography.empty()){
            vector<Point2f> markerCorners(4);

            markerCorners[0] = Point2f(0, 0);
            markerCorners[1] = Point2f(image.cols, 0);
            markerCorners[2] = Point2f(image.cols, image.rows);
            markerCorners[3] = Point2f(0, image.rows);

            perspectiveTransform(markerCorners, markerCorners, homography);

            const Scalar arrowColor = Scalar(113, 217, 125);
            arrowedLine(frame, markerCorners[0], markerCorners[1], arrowColor, 1, LINE_AA);
            arrowedLine(frame, markerCorners[1], markerCorners[2], arrowColor, 1, LINE_AA);

            const Scalar textColor = Scalar(128, 64, 32);
            putText(
                frame, format("%3.2f", measurements.height) + "cm",
                (markerCorners[1] + markerCorners[2]) / 2, FONT_HERSHEY_DUPLEX, .5,
                textColor, 1, LINE_AA
            );
            putText(
                frame, format("%3.2f", measurements.width) + "cm",
                (markerCorners[0] + markerCorners[1]) / 2, FONT_HERSHEY_DUPLEX, .5,
                textColor, 1, LINE_AA
            );

            // Returning the norms from the found segments
            norms.x = norm(Mat(markerCorners[0]), Mat(markerCorners[1]));
            norms.y = norm(Mat(markerCorners[1]), Mat(markerCorners[2]));
        }
    }
}


void Corner::resizeToFit(Mat &image){
    /*
     * Resizes an image (or frame) to fit the initialized dimensions
    */

    // The resize ratio, to later check if the frame must be resized or not.
    const double resizeRatio = resizeRespectingRatio(image.size());
    double maxContoursArea{0.}, currentArea;

    if (resizeRatio > .0)
        resize(image, image, Size(), resizeRatio, resizeRatio);
}


void Corner::measureObject(Mat &image, Point2f markerNorms){
    /*
     * Measures the object of interest, segmenting and localizing it.
    */
    const unsigned short int edgeThreshold = 100;
    double maxContoursArea = 0., currentArea;

    Mat edges, imageOneChannel;
    vector<Point> maxContour;
    vector<vector<Point>> contours;

    cvtColor(image, imageOneChannel, COLOR_BGR2GRAY);
    blur(imageOneChannel, imageOneChannel, Size(3, 3));
    Canny(imageOneChannel, edges, edgeThreshold, edgeThreshold * 3, 3, true);
    dilate(edges, edges, noArray());
    erode(edges, edges, noArray());
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);

    for (vector<Point> &contour: contours){
        currentArea = contourArea(contour);

        if (currentArea > maxContoursArea){
            maxContoursArea = currentArea;
            maxContour = contour;
        }
    }

    RotatedRect rotatedRectangle = minAreaRect(maxContour);
    Point2f rotatedBox[4];
    rotatedRectangle.points(rotatedBox);

    const Scalar rotatedRectangleColor = Scalar(245, 118, 219);
    line(image, rotatedBox[0], rotatedBox[1], rotatedRectangleColor, 1, LINE_AA);
    line(image, rotatedBox[1], rotatedBox[2], rotatedRectangleColor, 1, LINE_AA);
    line(image, rotatedBox[2], rotatedBox[3], rotatedRectangleColor, 1, LINE_AA);
    line(image, rotatedBox[3], rotatedBox[0], rotatedRectangleColor, 1, LINE_AA);

    const Size2f sizes = Size2f(
        norm(Mat(rotatedBox[0]), Mat(rotatedBox[1])) * markerSize.x / markerNorms.x,
        norm(Mat(rotatedBox[1]), Mat(rotatedBox[2])) * markerSize.y / markerNorms.y
    );

    const Scalar textColor = Scalar(34, 76, 173);
    putText(
        image, format("%3.2f", sizes.width) + "cm",
        (rotatedBox[0] + rotatedBox[1]) / 2, FONT_HERSHEY_DUPLEX, .5,
        textColor, 1, LINE_AA
    );
    putText(
        image, format("%3.2f", sizes.height) + "cm",
        (rotatedBox[1] + rotatedBox[2]) / 2, FONT_HERSHEY_DUPLEX, .5,
        textColor, 1, LINE_AA
    );
}
