#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    std::string calibrationPath = (argc > 1)
        ? argv[1]
        : "../ball_tracker/config/camera_calibration.yaml";

    // Load calibration file
    cv::FileStorage fs(calibrationPath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "❌ Failed to open calibration file: " << calibrationPath << std::endl;
        return 1;
    }

    cv::Mat cameraMatrix, distCoeffs;
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();

    std::cout << "📐 Calibration loaded successfully.\n";

    // Open camera
    cv::VideoCapture cam(0);
    if (!cam.isOpened()) {
        std::cerr << "❌ Failed to open camera.\n";
        return 1;
    }

    // Get frame size
    int width = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::Size frameSize(width, height);

    // Compute remapping for performance
    cv::Mat map1, map2;
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
                                cameraMatrix, frameSize, CV_16SC2, map1, map2);

    cv::Mat frame, undistorted, combined;
    while (true) {
        cam >> frame;
        if (frame.empty()) break;

        // Apply remap
        cv::remap(frame, undistorted, map1, map2, cv::INTER_LINEAR);

        // Side-by-side view
        cv::hconcat(frame, undistorted, combined);
        cv::putText(combined, "Original", {30, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 0, 255}, 2);
        cv::putText(combined, "Undistorted", {width + 30, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 255, 0}, 2);

        cv::imshow("Calibration Debug View", combined);
        if (cv::waitKey(1) == 27) break;  // ESC to exit
    }

    return 0;
}