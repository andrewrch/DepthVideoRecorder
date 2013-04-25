#include "depthvideorecorder.h"
#include "ui_depthvideorecorder.h"

#include <QMessageBox>
#include <QFileDialog>

DepthVideoRecorder::DepthVideoRecorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DepthVideoRecorder)
{
    ui->setupUi(this);

    // We're not recording yet.
    recording = false;
    framesRecorded = 0;

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
    connect(this, SIGNAL(disparityImageChanged(const cv::Mat&)),
            ui->disparityImageWidget, SLOT(setImage(const cv::Mat&)));
    connect(this, SIGNAL(validImageChanged(const cv::Mat&)),
            ui->validImageWidget, SLOT(setImage(const cv::Mat&)));

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
    if (ui)
        delete ui;
    if (timer)
        delete timer;
//    if (rgbVideo->isOpened())
//        delete rgbVideo;
//    if (depthVideo->isOpened())
//        delete depthVideo;
//    if (disparityVideo->isOpened())
//        delete disparityVideo;
//    if (validVideo->isOpened())
//        delete validVideo;
}

void DepthVideoRecorder::recordVideos()
{
    // Try opening all videos.
    rgbVideo.open(rgbFileName.toStdString());
    if (!rgbVideo.isOpened())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not open RGB video file");
        messageBox.setFixedSize(500,200);
        return;
    }
    depthVideo.open(depthFileName.toStdString());
    if (!depthVideo.isOpened())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not open depth video file");
        messageBox.setFixedSize(500,200);
        return;
    }
    disparityVideo.open(disparityFileName.toStdString());
    if (!disparityVideo.isOpened())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not open disparity video file");
        messageBox.setFixedSize(500,200);
        return;
    }
    validVideo.open(validFileName.toStdString());
    if (!validVideo.isOpened())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not open valid pixels video file");
        messageBox.setFixedSize(500,200);
        return;
    }

    // If all of that worked, set recording to true.
    recording = true;
    // Probably don't need to do this.
    frames = ui->frameCount->value();
    // Don't let anyone click button
    ui->goButton->setEnabled(false);

    // Tell progress bar to wake up
    emit progressUpdate(0);
}

/**
  *  Function grabs a single frame from the camera and
  *  then stores an RGB and depth image
  */
void DepthVideoRecorder::updateImages()
{
    // First capture images from the camera
    capture.grab();
    capture.retrieve(depthImage, CV_CAP_OPENNI_DEPTH_MAP);
    capture.retrieve(disparityImage, CV_CAP_OPENNI_DISPARITY_MAP);
    capture.retrieve(rgbImage, CV_CAP_OPENNI_BGR_IMAGE);
    capture.retrieve(validImage, CV_CAP_OPENNI_VALID_DEPTH_MASK);

    // Emit signals to display widgets
    emit rgbImageChanged(rgbImage);
    emit depthImageChanged(depthImage);
    emit disparityImageChanged(disparityImage);
    emit validImageChanged(validImage);

    if (recording)
    {
        rgbVideo << rgbImage;
        depthVideo << depthImage;
        disparityVideo << disparityImage;
        validVideo << validImage;

        framesRecorded++;

        int progress = (int) ((double) framesRecorded / frames * 100);
        emit progressUpdate(progress);

        if (framesRecorded == frames)
        {
            QMessageBox messageBox;
            messageBox.information(this,"Success","All files recorded successfully");
            messageBox.setFixedSize(500,200);

            recording = false;
//            delete rgbVideo;
//            delete depthVideo;
//            delete disparityVideo;
//            delete validVideo;
            emit progressUpdate(0);
            ui->goButton->setEnabled(true);
        }
    }
}

void DepthVideoRecorder::updateFileNamesDialog()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save RGB Video"),
                                ui->rgbFileName->text(),
                                tr("All Files (*)"),
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
    if (fileNameCopy.endsWith("_rgb"))
        fileNameCopy.chop(4);

    // Update filenames for storage with OpenCV
    rgbFileName       = fileNameCopy + "_rgb";
    depthFileName     = fileNameCopy + "_depth";
    disparityFileName = fileNameCopy + "_disp";
    validFileName     = fileNameCopy + "_valid";
    paramsFileName    = fileNameCopy + "_props";

    // Update the UI
    ui->rgbFileName->setText(rgbFileName);
    ui->depthFileName->setText(depthFileName);
    ui->disparityFileName->setText(disparityFileName);
    ui->validFileName->setText(validFileName);
    ui->paramsFileName->setText(paramsFileName);
}












