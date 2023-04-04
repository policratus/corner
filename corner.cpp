#include <iostream>
#include <filesystem>

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include "cornerlib/cornerlib.hpp"


using namespace filesystem;


int main(int argc, char** argv){
    /*
     * Corner - Calculates dimensions of multiple objects in scene in real-time.
     * Just use a image as marker and this marker must be also present in the scene.
    */

    Corner corner;

    Mat frame;
    VideoCapture video;

    // Command line arguments parsed
    string videoSource;
    path markerSource;
    float markerHeight;
    float markerWidth;

    tie(videoSource, markerSource, markerHeight, markerWidth) = corner.commandLine(argc, argv);

    // Loading the image marker.
    Mat markerImage = corner.getMarker(markerSource);
    cout << "Marker image has size " + to_string(markerImage.size().height) + "x" + to_string(markerImage.size().width) << endl;

    // If "camera" was the choice of video source
    if (videoSource == "camera"){
        const unsigned short int devices = corner.numberOfCameraDevices();

        // If found at least one camera
        if (devices > 0){
            if (devices == 1){ // If found just one camera, use it.
                video.open(devices - 1);
            } else {
                // More than one camera was found. User must choose one of them.
                unsigned short int chosenCamera;

                cout << to_string(devices) + " cameras found. Please select a camera between 1 and " + to_string(devices) + ": ";
                cin >> chosenCamera;

                // If selected camera are inside the camera ranges.
                if (chosenCamera >= 1 && chosenCamera <= devices)
                    video.open(chosenCamera);
                else {
                    cerr << "Camera device " + to_string(chosenCamera) + " is out of bounds." << endl;

                    return EXIT_FAILURE;
                }
            }
        } else {
            cerr << "No camera found. Exiting." << endl;

            return EXIT_FAILURE;
        }
    } else {
        // If was passed as argument a protocol video format (usually network devices and streaming).
        if (videoSource.find("://") != string::npos)
            video.open(videoSource);
        else {
            // The source passed is a video file.
            path videoSourcePath = path(videoSource);

            if (empty(videoSourcePath)){
                cerr << endl << "Source not defined for video." << endl;

                return EXIT_FAILURE;
            }
            if (!exists(videoSourcePath)){
                cerr << "File " + videoSourcePath.string() + " as video source not found." << endl;

                return EXIT_FAILURE;
            }

            video.open(videoSourcePath);
        }
    }

    if (!video.isOpened()){
        cerr << "Couldn't open video." << endl;

        return EXIT_FAILURE;
    }

    cout << "Marker height: " + to_string(markerHeight) + " cm." << endl;
    cout << "Marker width: " + to_string(markerWidth) + " cm." << endl;

    namedWindow("Corner", WINDOW_AUTOSIZE);

    double resizeRatio;
    // Video loop. Less things inside this loop, the better.
    while (true){
        video.read(frame);

        if (frame.empty()) break;

        // The resize ratio, to later check if the frame must be resized or not.
        resizeRatio = corner.resizeRespectingRatio(frame.size());

        if (resizeRatio > .0)
            resize(frame, frame, Size(), resizeRatio, resizeRatio);

        imshow("Corner", frame);

        if (waitKey(1) >= 0) break;
    }

    video.release();
    destroyAllWindows();

    return EXIT_SUCCESS;
}
