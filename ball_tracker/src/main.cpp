#include "camera.hpp"
#include "ball_tracker.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    Camera camera;
    BallTracker ballTracker;

    // 👇 Load calibration data
    if (!ballTracker.loadCalibration("../ball_tracker/config/camera_calibration.yaml")) {
        std::cerr << "⚠️  Continuing without camera calibration..." << std::endl;
    }
    

    cv::Mat frame;
    cv::Mat debugFrame;

    while (true) {
        if (!camera.getFrame(frame)) {
            std::cerr << "Failed to read frame from camera." << std::endl;
            break;
        }

        cv::Point ballPosition = ballTracker.detectBall(frame, debugFrame);
        if (ballPosition.x != -1) {
            std::cout << "🎯 Ball detected at: (" << ballPosition.x << ", " << ballPosition.y << ")" << std::endl;
        }

        cv::imshow("Ball Detection Debug View", debugFrame);
        if (cv::waitKey(1) == 27) {  // ESC key
            break;
        }
    }

    return 0;
}