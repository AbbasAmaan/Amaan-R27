cmake_minimum_required(VERSION 3.10)

project(RudraObjectDetection)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv-install/lib/cmake/opencv5")

find_package(OpenCV REQUIRED)

add_executable(object_detection src/main.cpp)

target_link_libraries(object_detection ${OpenCV_LIBS})

target_include_directories(object_detection PRIVATE ${OpenCV_INCLUDE_DIRS})
