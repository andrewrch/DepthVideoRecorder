#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <QDebug>

class VideoWriter
{
public:
    VideoWriter(std::string fileName)
    {
       qDebug() << fileName.c_str();
       theVideo.open(fileName.c_str(), std::ios::out | std::ios::binary);
       firstFrame = true;
    }

    ~VideoWriter() {}

    bool isOpened() { return !theVideo; }

    void write(const cv::Mat& frame)
    {
        // Need to determine type of images we're writing
        if (firstFrame)
        {
            frameType = frame.type();
            frameChannels = frame.channels();
            frameDepth = frame.depth();
            frameHeight = frame.rows;
            frameWidth = frame.cols;
            frameSize = frameChannels * frameDepth * frameWidth * frameHeight;
            theVideo.write(reinterpret_cast<char*>(frameType), sizeof(int));
            theVideo.write(reinterpret_cast<char*>(frameChannels), sizeof(int));
            theVideo.write(reinterpret_cast<char*>(frameDepth), sizeof(int));
            theVideo.write(reinterpret_cast<char*>(frameHeight), sizeof(int));
            theVideo.write(reinterpret_cast<char*>(frameWidth), sizeof(int));
            theVideo.write(reinterpret_cast<char*>(frameSize), sizeof(int));
            firstFrame = false;
        }

        theVideo.write(reinterpret_cast<char*>(frame.data), frameSize);
    }

private:
    std::ofstream theVideo;
    bool firstFrame;
    int frameType, frameChannels, frameDepth, frameWidth, frameHeight, frameSize;
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
       theVideo->read(reinterpret_cast<char*>(frameDepth), sizeof(int));
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
    int frameType, frameChannels, frameDepth, frameWidth, frameHeight, frameSize;
};

#endif // VIDEOSTREAM_H
