#pragma once
#include <opencv2/opencv.hpp>

class Camera {
public:
    Camera(int camIndex = 0);
    bool getFrame(cv::Mat& frame);
private:
    cv::VideoCapture cap;
};