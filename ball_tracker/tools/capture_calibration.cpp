// tools/capture_calibration.cpp
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cam(0);
    if (!cam.isOpened()) {
        std::cerr << "Camera not accessible" << std::endl;
        return -1;
    }

    int savedCount = 0;
    cv::Mat frame;
    while (true) {
        cam >> frame;
        if (frame.empty()) break;

        cv::imshow("Calibration Capture", frame);
        int key = cv::waitKey(1);
        if (key == 's') {
            std::string filename = "../ball_tracker/data/calibration/calib_" + std::to_string(savedCount++) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "Saved " << filename << std::endl;
        } else if (key == 27) break;
    }
    return 0;
}