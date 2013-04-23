#ifndef DEPTHVIDEORECORDER_H
#define DEPTHVIDEORECORDER_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <opencv2/opencv.hpp>

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
    void rgbImageChanged(const cv::Mat&);
    void progressUpdate(int);

private:
    Ui::DepthVideoRecorder *ui;
    QTimer *timer;

    void updateFileNames(const QString&);

    // CV device to record from Kinect
    cv::VideoCapture capture;
    // To hold the current RGB and depth images
    cv::Mat rgbImage, depthImage;

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
    QString rgbFileName, depthFileName, paramsFileName;
};

#endif // DEPTHVIDEORECORDER_H
