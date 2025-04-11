#include "ball_tracker.hpp"

BallTracker::BallTracker()
    : resolutionScale_(1.2),
      minCircleDistance_(30.0),
      edgeThreshold_(100.0),
      centerThreshold_(25.0),
      minRadius_(10),
      maxRadius_(50) { }

cv::Point BallTracker::detectBall(const cv::Mat& inputFrame, cv::Mat& debugFrame) {
    cv::Mat undistortedFrame;
    undistortFrame(inputFrame, undistortedFrame);
    debugFrame = undistortedFrame.clone();
    const cv::Mat preprocessed = preprocessFrame(undistortedFrame);
    const auto circles = findCircles(preprocessed);
    return drawBestCircle(circles, debugFrame);
}

bool BallTracker::loadCalibration(const std::string& calibrationFilePath) {
    cv::FileStorage fs(calibrationFilePath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "❌ Failed to open calibration file: " << calibrationFilePath << std::endl;
        return false;
    }

    fs["camera_matrix"] >> cameraMatrix_;
    fs["distortion_coefficients"] >> distortionCoefficients_;
    fs.release();

    if (cameraMatrix_.empty() || distortionCoefficients_.empty()) {
        std::cerr << "❌ Calibration data is invalid.\n";
        return false;
    }

    isCalibrated_ = true;
    std::cout << "✅ Calibration loaded successfully.\n";

    // Wait until we get a frame to initialize map
    frameSize_ = cv::Size();  // reset
    return true;
}

void BallTracker::undistortFrame(const cv::Mat& input, cv::Mat& output) {
    if (!isCalibrated_) {
        input.copyTo(output);
        return;
    }

    // Init remap if not done
    if (map1_.empty() || map2_.empty() || input.size() != frameSize_) {
        frameSize_ = input.size();
        cv::initUndistortRectifyMap(
            cameraMatrix_, distortionCoefficients_, cv::Mat(),
            cameraMatrix_, frameSize_, CV_16SC2, map1_, map2_);
    }

    cv::remap(input, output, map1_, map2_, cv::INTER_LINEAR);
}

cv::Mat BallTracker::preprocessFrame(const cv::Mat& input) {
    cv::Mat grayImage;
    cv::cvtColor(input, grayImage, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(grayImage, grayImage, cv::Size(9, 9), 2.0);
    return grayImage;
}

std::vector<cv::Vec3f> BallTracker::findCircles(const cv::Mat& preprocessed) {
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(
        preprocessed,
        circles,
        cv::HOUGH_GRADIENT,
        resolutionScale_,
        minCircleDistance_,
        edgeThreshold_,
        centerThreshold_,
        minRadius_,
        maxRadius_
    );
    return circles;
}

cv::Point BallTracker::drawBestCircle(const std::vector<cv::Vec3f>& circles, cv::Mat& debugFrame) {
    if (circles.empty()) {
        return {-1, -1};
    }

    const auto& circle = circles.front();
    const cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    const int radius = cvRound(circle[2]);

    cv::circle(debugFrame, center, radius, cv::Scalar(0, 255, 0), 2);
    cv::circle(debugFrame, center, 3, cv::Scalar(0, 0, 255), cv::FILLED);

    return center;
}