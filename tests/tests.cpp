#include <filesystem>

#include <catch2/catch.hpp>
#include <opencv2/core.hpp>

#include <libcorner.hpp>


using namespace cv;
using namespace filesystem;


const Size videoDimensions(1280, 720);
Corner corner(videoDimensions);


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

    SECTION("Image or frame resizing bigger than video dimensions."){
        const Size biggerSizeWidth = Size(videoDimensions.width * 2, videoDimensions.height / 2);
        Mat image(biggerSizeWidth, CV_8UC1, Scalar(0));

        corner.resizeToFit(image);

        REQUIRE(image.size() == Size(videoDimensions.width, videoDimensions.height / 4));
    }
}

TEST_CASE("Testing image and video IO"){

    SECTION("Opening an image marker."){
        path imagePath = path("tests") / "image.jpg";
        const Mat image = corner.getMarker(imagePath.string());

        REQUIRE(image.size() == Size(30, 22));
    }
}
