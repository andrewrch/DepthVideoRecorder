#include "depthvideorecorder.h"
#include "ui_depthvideorecorder.h"

#include <QMessageBox>
#include <QFileDialog>

DepthVideoRecorder::DepthVideoRecorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DepthVideoRecorder)
{
    ui->setupUi(this);

    // Set up the camera
    capture = cv::VideoCapture(CV_CAP_OPENNI);
    // If it fails throw up an error and quit
    if (!capture.isOpened())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not find an OpenNI device.  Click OK to exit");
        messageBox.setFixedSize(500,200);
        exit(1);
    }

    // Camera parameters
    // RGB Camera
    rgbWidth = capture.get(CV_CAP_OPENNI_IMAGE_GENERATOR + CV_CAP_PROP_FRAME_WIDTH);
    rgbHeight = capture.get(CV_CAP_OPENNI_IMAGE_GENERATOR + CV_CAP_PROP_FRAME_HEIGHT);
    rgbFps = capture.get(CV_CAP_OPENNI_IMAGE_GENERATOR + CV_CAP_PROP_FPS);

    // Depth Camera
    depthWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    depthHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    depthFps = capture.get(CV_CAP_PROP_FPS);
    baseline = capture.get(CV_CAP_OPENNI_DEPTH_GENERATOR_BASELINE);
    maxDepth = capture.get(CV_CAP_PROP_OPENNI_FRAME_MAX_DEPTH);
    focalLength = capture.get(CV_CAP_PROP_OPENNI_FOCAL_LENGTH);

    // Create a timer and make an idle function to update images
    // in the window.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateImages()));
    timer->start(0);

    // Connect signals to update the images
    connect(this, SIGNAL(depthImageChanged(const cv::Mat&)),
            ui->depthImageWidget, SLOT(setImage(const cv::Mat&)));
    connect(this, SIGNAL(rgbImageChanged(const cv::Mat&)),
            ui->rgbImageWidget, SLOT(setImage(const cv::Mat&)));

    // Connect signals to update fileNames
    connect(ui->browseButton, SIGNAL(clicked()),
            this, SLOT(updateFileNamesDialog()));
    connect(ui->rgbFileName, SIGNAL(editingFinished()),
            this, SLOT(updateFileNamesManual()));

    // Connect signals to update number of frames
    connect(ui->frameCount, SIGNAL(valueChanged(int)),
            this, SLOT(updateFrameCount(int)));

    // Connect signals to "Go!" button
    connect(ui->goButton, SIGNAL(clicked()),
            this, SLOT(recordVideos()));

    // Connect signals to progress bar
    connect(this, SIGNAL(progressUpdate(int)),
            ui->progressBar, SLOT(setValue(int)));
}

DepthVideoRecorder::~DepthVideoRecorder()
{
    delete ui;
    delete timer;
}

void DepthVideoRecorder::recordVideos()
{
}

/**
  *  Function grabs a single frame from the camera and
  *  then stores an RGB and depth image
  */
void DepthVideoRecorder::updateImages()
{
    // First capture images from the camera
    capture.grab();
//    capture.retrieve(depthImage, CV_CAP_OPENNI_DEPTH_MAP);
    capture.retrieve(depthImage, CV_CAP_OPENNI_DISPARITY_MAP);
    emit depthImageChanged(depthImage);
    capture.retrieve(rgbImage, CV_CAP_OPENNI_BGR_IMAGE);
    emit rgbImageChanged(rgbImage);
}

void DepthVideoRecorder::updateFileNamesDialog()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save RGB Video"),
                                ui->rgbFileName->text(),
                                tr("AVI Files (*.avi)"),
                                &selectedFilter,
                                options);

    if (!fileName.isEmpty())
        updateFileNames(fileName);
}

void DepthVideoRecorder::updateFileNamesManual()
{
    const QString& filename = ui->rgbFileName->text();

    // Only update if filename is changed
    if (filename != rgbFileName)
    {
        if (filename == "")
        {
            ui->depthFileName->setText("");
            ui->paramsFileName->setText("");
        }
        else
            updateFileNames(filename);
    }
}

void DepthVideoRecorder::updateFileNames(const QString& fileName)
{
    // Copy since param is const.
    QString fileNameCopy(fileName);

    // Remove any filename suffixes and other rubbish
    if (fileNameCopy.endsWith("_rgb.avi"))
        fileNameCopy.chop(8);
    else if (fileNameCopy.endsWith(".avi"))
        fileNameCopy.chop(4);

    // Update filenames for storage with OpenCV
    rgbFileName    = fileNameCopy + "_rgb.avi";
    depthFileName  = fileNameCopy + "_depth.avi";
    paramsFileName = fileNameCopy + ".props";

    // Update the UI
    ui->rgbFileName->setText(rgbFileName);
    ui->depthFileName->setText(depthFileName);
    ui->paramsFileName->setText(paramsFileName);
}



















