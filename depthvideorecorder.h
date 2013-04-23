#ifndef DEPTHVIDEORECORDER_H
#define DEPTHVIDEORECORDER_H

#include <QMainWindow>

namespace Ui {
class DepthVideoRecorder;
}

class DepthVideoRecorder : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DepthVideoRecorder(QWidget *parent = 0);
    ~DepthVideoRecorder();
    
private:
    Ui::DepthVideoRecorder *ui;
};

#endif // DEPTHVIDEORECORDER_H
