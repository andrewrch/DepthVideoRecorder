//#include "qcvmatwidget.h"
//#include "ui_qcvmatwidget.h"
//
//QCVMatWidget::QCVMatWidget(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::QCVMatWidget)
//{
//    ui->setupUi(this);
//}
//
//QCVMatWidget::~QCVMatWidget()
//{
//    delete ui;
//}

#include "qcvmatwidget.h"
#include "ui_qcvmatwidget.h"

QCVMatWidget::QCVMatWidget(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::QCVMatWidget)
{
    ui->setupUi(this);
    // This is required if you need to transmit IplImage over
    // signals and slots.(That's what I am doing in my application
    //qRegisterMetaType<IplImage>("IplImage");
    // Do something better here... (Pass W/H of cv::Mat to constructor?)
    //resize(384,288);

    this->parent = parent;
    //hidden = false;
    //bgColor = QColor::fromRgb(0xe0,0xdf,0xe0);
    bgColor = QColor::fromRgb(0xe0,0x00,0x00);

    initializeGL();
}

QCVMatWidget::~QCVMatWidget()
{
}

void QCVMatWidget::initializeGL()
{
    qglClearColor(bgColor);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void QCVMatWidget::setImage(const cv::Mat& image)
{
    GLenum imageFormat;
    switch(image.channels())
    {
        case 1:
            imageFormat = GL_LUMINANCE;
            break;
        case 2:
            imageFormat = GL_LUMINANCE_ALPHA;
            break;
        case 3:
        default:
            imageFormat = GL_BGR;
            break;
    }

    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set up the texture
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 this->width(),
                 this->height(),
                 0,
                 imageFormat,
                 GL_UNSIGNED_BYTE,
                 image.data);

    // Tell the widget to redraw
    updateGL();
}

void QCVMatWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Draw the textured quad
    glBegin(GL_QUADS);
        glTexCoord2i(0,1); glVertex2f(0.0f,1.0f);
        glTexCoord2i(0,0); glVertex2f(0.0f,0.0f);
        glTexCoord2i(1,0); glVertex2f(1.0f,0.0f);
        glTexCoord2i(1,1); glVertex2f(1.0f,1.0f);
    glEnd();

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glEnable(GL_DEPTH_TEST);
}

void QCVMatWidget::resizeGL(int width,int height)
{
    glViewport(0,0,this->width(),this->height());
}
