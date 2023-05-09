#include <iostream>
#include <filesystem>

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

#include "libcorner.hpp"


using namespace std;
using namespace filesystem;


// Store mouse positions
Point mouseClick(0, 0), mousePosition(0, 0);


static void onMouse(int event, int x, int y, int flags, void*){
    /*
     * Handles mouse events
    */
    switch (event){
        case EVENT_LBUTTONDOWN:
            mouseClick = Point(x, y);

        case EVENT_MOUSEMOVE:
            if (mouseClick.x != 0 && mouseClick.y != 0)
                mousePosition = Point(x, y);
    }
}


int main(int argc, char** argv){
    /*
     * Corner - Calculates dimensions of multiple objects in scene in real-time.
     * Just use a image as marker and this marker must be also present in the scene.
    */

    // Global frame capsule
    Mat frame;

    Corner corner(Size(1280, 720));

    VideoCapture video;

    string videoSource;
    path markerSource;

    // Command line arguments parsed
    tie(
        videoSource, markerSource,
        corner.markerSize.y, corner.markerSize.x
    ) = corner.commandLine(argc, argv);

    // Loading the image marker.
    Mat markerImage = corner.getMarker(markerSource);

    // If "camera" was the choice of video source
    if (videoSource == "camera"){
        unsigned short int devices(corner.numberOfCameraDevices());

        // If found at least one camera
        if (devices > 0){
            if (devices == 1){ // If found just one camera, use it.
                video.open(--devices);
            } else {
                // More than one camera was found. User must choose one of them.
                unsigned short int chosenCamera;

                cout << to_string(devices) + " cameras found. Please select a camera between 0 and " +
                    to_string(devices) + ": ";
                cin >> chosenCamera;

                // If selected camera are inside the camera ranges.
                if (chosenCamera <= devices)
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
            // Video file not found
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

    namedWindow("Corner", WINDOW_AUTOSIZE);
    // Binding the callback function to window
    setMouseCallback("Corner", onMouse);

    // Store the norm of the two segments from marker
    Point2f norms;

    time_t lastTime, currentTime;
    // Store the current timestamp
    time(&lastTime);

    // Video loop. Less things inside this loop, the better.
    while (true){
        video.read(frame);

        // If the video stream ended.
        if (frame.empty()) break;

        corner.resizeToFit(frame);

        time(&currentTime);

        // If at least one second passed from the last check
        if (currentTime > lastTime){
            corner.findMarker(markerImage, frame, corner.markerSize, norms);
            time(&lastTime);
        }

        corner.drawMeasurementsObject(frame, norms);

        // Draw a segment similar to a ruler
        if (mouseClick.dot(mousePosition) > 0){
            line(frame, mouseClick, mousePosition, Scalar(99, 204, 67), 2, LINE_AA);
            putText(
                frame, format("%3.2f", corner.measure(mouseClick, mousePosition, norms.x)) + "cm",
                (mouseClick + mousePosition) / 2, FONT_HERSHEY_DUPLEX,
                .5, Scalar(104, 70, 67), 1, LINE_AA
            );
        }

        imshow("Corner", frame);

        if (waitKey(1) >= 0) break;
    }

    video.release();
    destroyAllWindows();

    return EXIT_SUCCESS;
}
