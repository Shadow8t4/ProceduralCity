
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(citygen)

file(GLOB_RECURSE HEADER_CODE include/*.h)
file(GLOB_RECURSE SRC_CODE source/*.cpp)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SRC_CODE} ${HEADER_CODE})

