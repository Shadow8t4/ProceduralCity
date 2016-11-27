
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(citygen)

file(GLOB_RECURSE HEADER_CODE ${citygen_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SRC_CODE ${citygen_SOURCE_DIR}/source/*.cpp)

ADD_EXECUTABLE(citygen ${SRC_CODE} ${HEADER_CODE})

