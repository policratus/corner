#include <filesystem>

#include <catch2/catch.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <libcorner.hpp>


using namespace cv;
using namespace filesystem;


const Size videoDimensions(1280, 720);
Corner corner(videoDimensions);
path testsPath(path("tests"));


TEST_CASE("Testing Corner class initialization and members"){

    SECTION("Testing marker size attribuition"){
        corner.markerSize = Point2f(1, 1);

        REQUIRE(corner.markerSize == Point2f(1, 1));
    }
}

TEST_CASE("Testing resizing methods"){

    SECTION("Image or frame resizing smaller than video dimensions."){
        const Size smallerSize(videoDimensions / 2);
        Mat image(smallerSize, CV_8UC1, Scalar(0));

        corner.resizeToFit(image);

        REQUIRE(image.size() == smallerSize);
    }

    SECTION("Image or frame resizing bigger than video dimensions."){
        const Size biggerSize(videoDimensions * 2);
        Mat image(biggerSize, CV_8UC1, Scalar(0));

        corner.resizeToFit(image);

        REQUIRE(image.size() == videoDimensions);
    }

    SECTION("Image or frame resizing width bigger than video dimensions."){
        const Size biggerSizeWidth = Size(videoDimensions.width * 2, videoDimensions.height / 2);
        Mat image(biggerSizeWidth, CV_8UC1, Scalar(0));

        corner.resizeToFit(image);

        REQUIRE(image.size() == Size(videoDimensions.width, videoDimensions.height / 4));
    }
}

TEST_CASE("Testing the detection pipeline"){

    const path markerPath = testsPath / "marker.jpg";
    const path imagePath = testsPath / "detect.jpg";

    const Mat markerImage(corner.getMarker(markerPath));
    const Mat imageDetection(corner.getMarker(imagePath));

    const RotatedRect box(corner.boundingBox(imageDetection));

    Point2f norms;

    Mat detectImage = corner.getMarker(imagePath);

    corner.findMarker(markerImage, detectImage, Size2f(3, 4), norms);

    SECTION("Detect marker, bound it and return norms"){
        const Point expectedNorm(73, 92);

        // Avoiding float comparisons
        REQUIRE(Point(norms) == expectedNorm);
    }

    SECTION("Detect the object bounding box."){

        REQUIRE(Size(box.size) == Size(632, 631));
        REQUIRE((int)box.angle == 89);
        REQUIRE(Point(box.center) == Point(646, 365));
    }

    SECTION("Measurements calculation."){

        Point2f rotatedBox[4];
        box.points(rotatedBox);
        const Size2f dimensions(corner.measure(rotatedBox, norms));

        // Avoiding float comparisons
        REQUIRE(Size(dimensions) == Size(10, 5));
    }

    SECTION("Measurements of two points."){

        Point firstPoint(0, 0);
        Point secondPoint(1, 1);
        unsigned short int markerNormX(1);

        corner.markerSize = Point2f(1, 1);

        // Avoiding float comparisons
        REQUIRE((int)corner.measure(firstPoint, secondPoint, markerNormX) == 1);
    }

    SECTION("Check measurements drawing"){
        Mat imageToDetect;
        imageDetection.copyTo(imageToDetect);
        corner.drawMeasurementsObject(imageToDetect, norms);

        REQUIRE(!imageToDetect.empty());
    }
}

TEST_CASE("Testing image and video IO"){

    SECTION("Opening an image marker."){
        path imagePath = testsPath / "image.jpg";
        const Mat image = corner.getMarker(imagePath.string());

        REQUIRE(image.size() == Size(30, 22));
    }

    SECTION("Trying to open an non-existent image marker."){
        path imagePath = testsPath / "thisimagenotexists.jpg";

        REQUIRE_THROWS(corner.getMarker(imagePath.string()));
    }

    SECTION("Looking for camera devices."){
        const unsigned short int cameras = corner.numberOfCameraDevices();

        REQUIRE(cameras == 0);
    }
}
