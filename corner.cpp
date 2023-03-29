#include <iostream>
#include <filesystem>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "cornerlib/cornerlib.hpp"


using namespace filesystem;


int main(int argc, char** argv){

    Corner corner;

    Mat frame;
    VideoCapture video;

    const String keys =
        "{help h usage ?||Help and usage for Corner}"
        "{@marker|<none>|Path where the image marker are stored}"
        "{@video|<none>|Path to the video file, streaming URI or webcam}"
        "{height|5.0|Height of the marker}"
        "{width|5.0|Width of the marker}"
        ;

    CommandLineParser parser(argc, argv, keys);
    parser.about("📏 Corner virtual rulers.");

    if (parser.has("help")){
        parser.printMessage();

        return EXIT_SUCCESS;
    }

    if (
        !parser.has("@marker") || !parser.has("@video") || !parser.check()
    ){
        parser.printErrors();
        parser.printMessage();

        return EXIT_FAILURE;
    }

    // Command line arguments parsed
    string videoSource = parser.get<String>("@video");

    if (videoSource == "camera"){
        unsigned short int devices = corner.numberOfCameraDevices();

        if (devices > 0){
            if (devices == 1){
                video.open(--devices);
            } else {
                unsigned short int chosenCamera;

                cout << to_string(devices) + " cameras found. Please select a camera between 1 and " + to_string(devices) + ": ";
                cin >> chosenCamera;

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
        if (videoSource.find("://") != string::npos)
            video.open(videoSource);
        else {
            path videoSourcePath = path(videoSource);

            if (!exists(videoSourcePath)){
                cerr << "File " + videoSourcePath.string() + " as video source not found." << endl;

                return EXIT_FAILURE;
            }

            video.open(videoSourcePath);
        }
    }

    path markerSource = parser.get<String>("@marker");
    // float markerHeight = parser.get<float>("height");
    // float markerWidth = parser.get<float>("width");

    if (!exists(markerSource)){
        cerr << "Image marker " + markerSource.string() + " not found." << endl;

        return EXIT_FAILURE;
    }

    if (!video.isOpened()){
        cerr << "Couldn't open video." << endl;
        return EXIT_FAILURE;
    }

    while (true){
        video.read(frame);

        if (frame.empty()) break;

        namedWindow("Corner", WINDOW_AUTOSIZE);
        imshow("Corner", frame);

        if (waitKey(1) >= 0) break;
    }

    video.release();
    destroyAllWindows();

    return EXIT_SUCCESS;
}
