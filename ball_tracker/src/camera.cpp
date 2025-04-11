#include "camera.hpp"
#include <iostream>

Camera::Camera(int camIndex) {
    // Use V4L2 backend
    cap.open(camIndex, cv::CAP_V4L2);

    if (!cap.isOpened()) {
        std::cerr << "❌ Could not open camera at index " << camIndex << std::endl;
        return;
    }

    // Request MJPEG format and 1080p @ 30fps
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(cv::CAP_PROP_FPS, 30);

    // Show what we actually got
    std::cout << "🎥 Camera initialized:" << std::endl;
    std::cout << "Resolution: " << cap.get(cv::CAP_PROP_FRAME_WIDTH)
              << "x" << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    std::cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << std::endl;
}

bool Camera::getFrame(cv::Mat& frame) {
    if (!cap.isOpened()) return false;
    cap >> frame;
    return !frame.empty();
}