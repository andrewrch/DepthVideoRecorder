#include "depthvideorecorder.h"
#include "ui_depthvideorecorder.h"

DepthVideoRecorder::DepthVideoRecorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DepthVideoRecorder)
{
    ui->setupUi(this);
}

DepthVideoRecorder::~DepthVideoRecorder()
{
    delete ui;
}
