cmake_minimum_required(VERSION 3.5)
project(HelloDarknet
  VERSION 0.1.0)

add_definitions(-std=c++11)

option(USE_GPU "Use GPU as image inference" OFF)

#########################################################################
set(CMAKE_BUILD_TYPE "Debug")
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "No build type selected, default to ${CMAKE_BUILD_TYPE}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fPIC")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fPIC")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")
#########################################################################

find_package(OpenCV REQUIRED)

if (USE_GPU)
  add_definitions(-DGPU)
  add_definitions(-DCUDNN)
  set(CUDA_root "/usr/local/cuda")
  set(CUDA_INCLUDE_DIR ${CUDA_root}/include)
  find_library(CUDART cudart "${CUDA_root}/lib64")
  find_library(CUBLAS cublas "${CUDA_root}/lib64")
  find_library(CURAND curand "${CUDA_root}/lib64")
  find_library(CUDNN cudnn "${CUDA_root}/lib64")
  set(CUDA_LIBS ${CUDART} ${CUBLAS} ${CURAND} ${CUDNN})
endif(USE_GPU)

set(Darknet_LIB
  ${PROJECT_SOURCE_DIR}/lib/libdarknet.so
)

###########
## Build ##
###########

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
  set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
endif()

include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
  ${CUDA_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME}
  src/main.cc
  src/DarknetDetector.cc
)

if (USE_GPU)
  target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${Darknet_LIB}
    ${CUDA_LIBS}
    lua
    glog
    -Wl,--no-whole-archive
  )
else ()
  target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${Darknet_LIB}
    glog
    gflags
    -Wl,--no-whole-archive
  )
endif(USE_GPU)
