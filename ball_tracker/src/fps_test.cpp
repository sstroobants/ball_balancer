#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    // Try to open /dev/video0 with MJPEG format
    cv::VideoCapture cap;

    // Use CAP_V4L2 to make sure we use the correct backend
    cap.open(0, cv::CAP_V4L2);

    if (!cap.isOpened()) {
        cerr << "❌ Could not open the camera." << endl;
        return -1;
    }

    // Set MJPEG + resolution + FPS
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 60);

    // Print what the camera actually accepted
    cout << "🎥 Actual settings:" << endl;
    cout << "Resolution: "
         << cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x"
         << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << endl;

    cv::Mat frame;
    int frameCount = 0;
    auto start = high_resolution_clock::now();

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        frameCount++;

        // Show the image (optional, can comment this out to benchmark raw capture)
        // cv::imshow("Camera FPS Test", frame);
        // if (cv::waitKey(1) == 27) break; // ESC to exit

        auto now = high_resolution_clock::now();
        double elapsed = duration_cast<duration<double>>(now - start).count();

        if (elapsed >= 1.0) {
            cout << "📸 FPS: " << frameCount / elapsed << endl;
            frameCount = 0;
            start = now;
        }
    }

    return 0;
}