DepthVideoRecorder
==================

First QT app.  Records data from OpenNI Depth camera as binary file (Using OpenCV to hand images and capture)

Usage
=====

This is a QTCreator project.  I haven't used it before so no idea how to compile outside of QTCreator, but it shouldn't be too difficult (?!).

Just launches a single window, which displays video from an OpenNI device (These are drawn as textured quads using a widget derived from QGLWidget).

Setting the RGB filename then sets all the others, and clicking the Go! button saves 4 video streams to binary files.

Haven't really paid much attention to file format:  It's just a few parameters (framesize etc) followed by the raw cv::Mat data.

WARNING
=======
This has hardly been tested so if in the unlikely event that someone out there really uses this, don't expect it to be bug free :).
