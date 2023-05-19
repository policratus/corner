[![CI](https://github.com/policratus/corner/actions/workflows/ci.yml/badge.svg)](https://github.com/policratus/corner/actions/workflows/ci.yml)
[![CD](https://github.com/policratus/corner/actions/workflows/cd.yml/badge.svg)](https://github.com/policratus/corner/actions/workflows/cd.yml)
[![codecov](https://codecov.io/gh/policratus/corner/branch/main/graph/badge.svg?token=5cmuypkroI)](https://codecov.io/gh/policratus/corner)

![corner](docs/corner.png)

# corner

### Real-time 2D measurements of objects based on visual markers.
---

![output](https://github.com/policratus/corner/assets/827563/7aead798-5787-463e-89de-d32c9234f164)

## 📐 What `corner` does?
`corner` is a measurement tool using only visual references present in videos, without the need of specialized hardware like [LIDAR](https://en.wikipedia.org/wiki/Lidar) sensors or any kind of [RBG-D](https://www.e-consystems.com/blog/camera/technology/what-are-rgbd-cameras-why-rgbd-cameras-are-preferred-in-some-embedded-vision-applications) devices (despite having less accuracy). It only needs a [fiducial marker](https://en.wikipedia.org/wiki/Fiducial_marker) inside the scene (with known dimensions), which can be from a simple ArUco marker, a book cover or even a [banana](https://knowyourmeme.com/memes/banana-for-scale).

It has also an interactive ruler that can be used as a complementary virtual one to measure new segments, going beyong the infered dimensions automatically calculated. Just click or touch on some part of the video and the ruler appears.

## 🎉 Getting started
### 📦 Installation
#### Binary packages
Download a package supported by your system in the [releases](https://github.com/policratus/corner/releases/latest) page (more binary packages coming soon). Some installation instructions over the current supported ones:
##### Ubuntu/Debian based
Go to the latest [release](https://github.com/policratus/corner/releases/latest) and download (from the assets) the package named `corner_[version]_amd64.deb` and after the successful download:

```shell
# This example supposes version 0.2.3. Please change it to the version being installed.
sudo apt install corner_0.2.3_amd64.deb -y
```

#### Manual (compilation)
If any specific needs comes up, it is also pretty easy and straightforward to compile `corner` on most platforms. Since it uses [`cmake`](https://cmake.org), it's a matter of calling the right entries:

```shell
git clone git@github.com:policratus/corner.git
cmake -S corner -B corner/build
cmake --build corner/build
cmake --install corner/build
```

##### Dependencies
The main dependency for `corner` is [opencv](https://opencv.org). Manual installation has the implication that you must deal with dependency installation. Here some instructions to help with this:

###### Ubuntu/Debian based

```shell
sudo apt install ^libopencv -y
```

### 🚸 Usage
Everything starts calling `corner`:

```shell
📐 Corner: measuring objects using only images.
Usage: corner [params] marker video

	-?, -h, --help, --usage
		Help and usage for Corner
	--height (value:5.0)
		Height of the marker (in cm.)
	--width (value:5.0)
		Width of the marker (in cm.)

	marker (value:<none>)
		Path where the image marker are stored
	video (value:<none>)
		Path to the video file, streaming URI or camera
```

so the one-liner example usage is described as:

```shell
corner marker.jpg video.mp4 --height=3.1 --width=4.2
```

where in the example above `corner` finds where in the current frame of `video.mp4` (can be also a streaming network camera or a webcam) the `marker.jpg` is located, measures it with known dimensions defined as a `--height` of 3.1 cm and `--width` 4.2 cm and extrapolates the measurements to the object of interest, usually larger than the marker.

### ⚠️ Disclaimer
`corner` is a project focused to be lightweight and run fast on virtually any machine, but the current version has some limitations that must be advised (hope to remove these in subsequent versions):

* The object intended to discover the measurements must be just one, so unfortunately more than one object is not supported yet (not to be confused with the marker);
* Right now `corner` uses a simple (but blazing fast) segmentation algorithm, which can run at hundreds of frames per second (fps) on simple machines but is not stable (and precise) as more complex ones. For best results, put the marker and interest object on uniform backgrounds, like a white, green, or any other with not so complex patterns;
* The marker localization process happens every one second, this is why a quick freeze is perceived at this pace. This maintains the fps stable even on weak machines.

Follow `corner`'s [project board](https://github.com/users/policratus/projects/1) and know its roadmap and future improvements.

### 🖊️ Citation
If you use `corner` in your publications or projects, please cite:

```BibTeX
@misc{corner,
    author = {Nelson Forte de Souza Junior},
    title = {corner - Real-time 2D measurements of objects based on visual markers.},
    howpublished = {\url{https://github.com/policratus/corner}},
    year = {2023}
}
```
