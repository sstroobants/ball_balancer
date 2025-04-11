#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

int main() {
    const int boardCols = 9;   // Inner corners
    const int boardRows = 6;
    const cv::Size patternSize(boardCols, boardRows);

    std::vector<std::vector<cv::Point3f>> objectPoints; // 3D points
    std::vector<std::vector<cv::Point2f>> imagePoints;  // 2D image points

    // Real-world object coordinates (Z=0 plane)
    std::vector<cv::Point3f> objectTemplate;
    for (int row = 0; row < boardRows; ++row)
        for (int col = 0; col < boardCols; ++col)
            objectTemplate.emplace_back(col, row, 0.0f);

    std::vector<std::string> imageFiles;
    for (const auto& entry : std::filesystem::directory_iterator("../ball_tracker/data/calibration/")) {
        if (entry.path().extension() == ".jpg") {
            imageFiles.push_back(entry.path().string());
        }
    }

    std::cout << "🔍 Found " << imageFiles.size() << " calibration images.\n";

    for (const auto& file : imageFiles) {
        cv::Mat image = cv::imread(file);
        if (image.empty()) continue;

        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(image, patternSize, corners,
                        cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

        cv::Mat display = image.clone();

        if (found) {
            cv::Mat gray;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

            cv::drawChessboardCorners(display, patternSize, corners, found);
        }

        std::string filename = std::filesystem::path(file).filename().string();
        cv::putText(display, "File: " + filename, {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 255, 255}, 2);

        if (!found)
            cv::putText(display, "❌ Corners NOT found!", {10, 60}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 0, 255}, 2);
        else
            cv::putText(display, "✅ Press Enter to accept, ESC to skip", {10, 60}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255, 255, 255}, 1);

        cv::imshow("Corner Detection", display);
        int key = cv::waitKey(0);

        if (found && key == 13) {  // Enter key
            objectPoints.push_back(objectTemplate);
            imagePoints.push_back(corners);
            std::cout << "✅ Accepted: " << filename << "\n";
        } else {
            std::cout << "⏭️ Skipped: " << filename << "\n";
        }
    }

    if (imagePoints.size() < 5) {
        std::cerr << "❌ Not enough valid calibration images.\n";
        return -1;
    }

    // Run calibration
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    cv::Size imageSize = cv::imread(imageFiles[0]).size();

    std::cout << "Found resolution " << imageSize.width << "x" << imageSize.height << "\n";

    double error = cv::calibrateCamera(objectPoints, imagePoints, imageSize,
                                       cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "\n📐 Calibration complete\n";
    std::cout << "Reprojection error: " << error << "\n";
    std::cout << "Camera matrix:\n" << cameraMatrix << "\n";
    std::cout << "Distortion coefficients:\n" << distCoeffs << "\n";

    // Save to file
    cv::FileStorage fs("../ball_tracker/config/camera_calibration.yaml", cv::FileStorage::WRITE);
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs.release();

    std::cout << "📁 Calibration saved to: ball_tracker/config/camera_calibration.yaml\n";
    return 0;
}