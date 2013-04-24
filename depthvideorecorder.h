#ifndef DEPTHVIDEORECORDER_H
#define DEPTHVIDEORECORDER_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <opencv2/opencv.hpp>

#include "videostream.h"

namespace Ui {
class DepthVideoRecorder;
}

class DepthVideoRecorder : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DepthVideoRecorder(QWidget *parent = 0);
    ~DepthVideoRecorder();

public slots:
    // Function to update the image widgets
    void recordVideos();
    void updateImages();
    void updateFileNamesDialog();
    void updateFileNamesManual();
    // void updateProgressBar(int);
    void updateFrameCount(int f) { frames = f; }

signals:
    void depthImageChanged(const cv::Mat&);
    void disparityImageChanged(const cv::Mat&);
    void rgbImageChanged(const cv::Mat&);
    void validImageChanged(const cv::Mat&);
    void progressUpdate(int);

private:
    void updateFileNames(const QString&);

    Ui::DepthVideoRecorder *ui;
    QTimer *timer;


    // CV device to record from Kinect
    cv::VideoCapture capture;
    // To hold the current RGB and depth images
    cv::Mat rgbImage, depthImage, disparityImage, validImage;
    // To hold store the videos
    //cv::VideoWriter *rgbVideo, *depthVideo, *disparityVideo, *validVideo;
    VideoWriter *rgbVideo, *depthVideo, *disparityVideo, *validVideo;

    // Has the "Go" button been pressed?
    bool recording;
    // How many frames have we recorded
    int framesRecorded;

    // Camera properties
    // RGB camera properties
    int rgbWidth, rgbHeight, rgbFps;
    // Depth camera properties
    int depthWidth, depthHeight, depthFps;
    float focalLength, baseline, maxDepth;
    // Camera registration (Calibrated depth and RGB?)
    bool registration;

    // Frames we'll record
    int frames;
    // Filenames
    QString rgbFileName, depthFileName, paramsFileName,
        validFileName, disparityFileName;
};

#endif // DEPTHVIDEORECORDER_H
