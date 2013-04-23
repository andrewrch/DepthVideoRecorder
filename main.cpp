#include <QApplication>
#include "depthvideorecorder.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DepthVideoRecorder w;
    w.show();
    
    return a.exec();
}
