#include "qcvmatwidget.h"
#include "ui_qcvmatwidget.h"

QCVMatWidget::QCVMatWidget(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::QCVMatWidget)
{
    ui->setupUi(this);
    this->parent = parent;
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
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void QCVMatWidget::setImage(const cv::Mat& image)
{
    GLenum internalFormat = GL_RGBA;
    GLenum inputType, inputColourFormat;
    switch(image.channels())
    {
        case 1:
            inputColourFormat = GL_LUMINANCE;
            if (image.depth() == CV_8U)
                inputType = GL_UNSIGNED_BYTE;
            else
                inputType = GL_UNSIGNED_SHORT;
            break;
        case 3:
        default:
            inputType = GL_UNSIGNED_BYTE;
            inputColourFormat = GL_BGR;
            break;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Set up the texture
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 image.cols,
                 image.rows,
                 0,
                 inputColourFormat,
                 inputType,
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
        glTexCoord2i(0, 0); glVertex2f(-1.0f,  1.0f);
        glTexCoord2i(0, 1); glVertex2f(-1.0f, -1.0f);
        glTexCoord2i(1, 1); glVertex2f( 1.0f, -1.0f);
        glTexCoord2i(1, 0); glVertex2f( 1.0f,  1.0f);
    glEnd();

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glEnable(GL_DEPTH_TEST);
}

void QCVMatWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
