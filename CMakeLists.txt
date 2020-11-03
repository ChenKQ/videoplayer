cmake_minimum_required(VERSION 3.5)

project(videoplayer LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

#add_subdirectory(brick)
find_package(GLOG REQUIRED)
find_package(BRICK REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
list(APPEND LINK_LIBS "Qt5::Core" "Qt5::Widgets"
    "Qt5::Gui")

add_executable(videoplayer main.cpp videoplayer.h videoplayer.cpp)
#target_include_directories(videoplayer PUBLIC "brick/include")
target_link_libraries(videoplayer PRIVATE brick ${LINK_LIBS} glog::glog)

add_executable(glvideoplay glvideoplayer.h glvideoplayer.cpp resources.qrc)
target_link_libraries(glvideoplay PRIVATE brick ${LINK_LIBS} opengl32 glog::glog)
