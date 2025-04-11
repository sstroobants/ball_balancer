cmake_minimum_required(VERSION 3.10)
project(BallTracker)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(ball_tracker
    src/main.cpp
    src/ball_tracker.cpp
    src/camera.cpp
)

add_executable(fps_test
    src/fps_test.cpp
)

add_executable(capture_calibration
    tools/capture_calibration.cpp
)

add_executable(run_calibration
    tools/run_calibration.cpp
)

add_executable(debug_undistort 
    tools/debug_undistort.cpp
)

target_link_libraries(ball_tracker ${OpenCV_LIBS})
target_link_libraries(fps_test ${OpenCV_LIBS})
target_link_libraries(capture_calibration ${OpenCV_LIBS})
target_link_libraries(run_calibration ${OpenCV_LIBS})
target_link_libraries(debug_undistort ${OpenCV_LIBS})