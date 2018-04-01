cmake_minimum_required(VERSION 3.4)
project(image)

#Headers 
include_directories(include)

#Sources 
set(SOURCES src/main.cpp src/image.cpp)

#Global files 
file(GLOB SOURCES "src/*.*")

#Find package 
find_package( OpenCV REQUIRED )

#Runnable 
add_executable( run  ${SOURCES} )

target_link_libraries( run  ${OpenCV_LIBS} )
