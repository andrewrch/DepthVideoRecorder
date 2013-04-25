#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

#include <QDebug>

class VideoWriter
{
public:
    VideoWriter() : firstFrame(true)
    {}

    VideoWriter(std::string fileName) : firstFrame(true)
    {
        open(fileName);
    }

    VideoWriter& operator<< (const cv::Mat& frame)
    {
        write(frame);
        return *this;
    }

    ~VideoWriter()
    {
    }

    void open(std::string fileName)
    {
        using std::ios;
        if (theVideo.is_open())
            theVideo.close();

        theVideo.open(fileName.c_str(), ios::out | ios::binary);
    }

    bool isOpened() { return theVideo.is_open() && !theVideo.fail(); }

    void write(const cv::Mat& frame)
    {
        // Need to determine type of images we're writing
        if (firstFrame)
        {
            frameType = frame.type();
            frameChannels = frame.channels();
            frameElemSize = frame.elemSize();
            frameHeight = frame.rows;
            frameWidth = frame.cols;
            frameSize = frameElemSize * frameWidth * frameHeight;

            theVideo.write(reinterpret_cast<const char*>(&frameType), sizeof(frameType));
            theVideo.write(reinterpret_cast<const char*>(&frameChannels), sizeof(frameChannels));
            theVideo.write(reinterpret_cast<const char*>(&frameElemSize), sizeof(frameElemSize));
            theVideo.write(reinterpret_cast<const char*>(&frameHeight), sizeof(frameHeight));
            theVideo.write(reinterpret_cast<const char*>(&frameWidth), sizeof(frameWidth));
            theVideo.write(reinterpret_cast<const char*>(&frameSize), sizeof(frameSize));
            firstFrame = false;
        }

        theVideo.write(reinterpret_cast<const char*>(frame.data), frameSize);
    }

private:
    std::fstream theVideo;
    bool firstFrame;
    int frameType, frameChannels, frameElemSize, frameWidth, frameHeight, frameSize;
};


class VideoReader
{
public:
    VideoReader(std::string fileName)
    {
       theVideo = new std::ifstream(fileName.c_str(), std::ios::in | std::ios::binary);
       // Get all info from start of file
       theVideo->read(reinterpret_cast<char*>(frameType), sizeof(int));
       theVideo->read(reinterpret_cast<char*>(frameChannels), sizeof(int));
       theVideo->read(reinterpret_cast<char*>(frameElemSize), sizeof(int));
       theVideo->read(reinterpret_cast<char*>(frameHeight), sizeof(int));
       theVideo->read(reinterpret_cast<char*>(frameWidth), sizeof(int));
       theVideo->read(reinterpret_cast<char*>(frameSize), sizeof(int));

       buffer = new char[frameSize];
    }

    ~VideoReader()
    {
        if (theVideo)
            delete theVideo;
        if (buffer)
            delete[] buffer;
    }
    cv::Mat read()
    {
        theVideo->read(buffer, frameSize);
        return cv::Mat(cv::Size(frameWidth, frameHeight), frameType, buffer);
    }

private:
    std::ifstream* theVideo;
    char* buffer;
    int cvType;
    int frameType, frameChannels, frameElemSize, frameWidth, frameHeight, frameSize;
};

#endif // VIDEOSTREAM_H
