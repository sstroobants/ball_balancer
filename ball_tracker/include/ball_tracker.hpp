#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class BallTracker {
public:
    BallTracker();

    /**
     * @brief Detects a ball in the input frame and draws debug output.
     * @param inputFrame The original BGR frame from the camera
     * @param debugFrame The frame where debug drawings will be shown
     * @return The center of the detected ball, or (-1, -1) if not found
     */
    cv::Point detectBall(const cv::Mat& inputFrame, cv::Mat& debugFrame);
    bool loadCalibration(const std::string& calibrationFilePath);
    void undistortFrame(const cv::Mat& input, cv::Mat& output);

private:
    // Hough parameters
    double resolutionScale_;
    double minCircleDistance_;
    double edgeThreshold_;
    double centerThreshold_;
    int minRadius_;
    int maxRadius_;

    // Camera calibration
    bool isCalibrated_;
    cv::Mat cameraMatrix_;
    cv::Mat distortionCoefficients_;
    cv::Mat map1_, map2_;     // Remap maps
    cv::Size frameSize_;      // Set during first undistort

    // Steps in pipeline
    cv::Mat preprocessFrame(const cv::Mat& input);
    std::vector<cv::Vec3f> findCircles(const cv::Mat& preprocessed);
    cv::Point drawBestCircle(const std::vector<cv::Vec3f>& circles, cv::Mat& debugFrame);
};