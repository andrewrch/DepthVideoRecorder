#ifndef QCVMATWIDGET_H
#define QCVMATWIDGET_H

#include <QGLWidget>
#include <opencv2/opencv.hpp>

namespace Ui {
    class QCVMatWidget;
}

class QCVMatWidget : public QGLWidget
{
    Q_OBJECT

public:
    // Constructor just takes pointer to parent
    explicit QCVMatWidget(QWidget *parent = 0);
    ~QCVMatWidget();

    // Overriden QGLWidget functions
    void paintGL();
    void initializeGL();
    void resizeGL(int, int);

public slots:
    // Callback to set the image to be displayed
    void setImage(const cv::Mat&);

private:
    Ui::QCVMatWidget* ui;
    // Contains a copy of the image we're drawing
    cv::Mat original;
    // Pointer to the texture
    GLuint texture;
    // Background colour if drawing fails
    QColor bgColor;
    QList<QPointF*> slopes;
    QWidget* parent;
};

#endif // QCVMATWIDGET_H
