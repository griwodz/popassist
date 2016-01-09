cmake_minimum_required(VERSION 3.4)
project( PopAssist )

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions("-DCUB_CDP")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules")

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions")

  # avoid the cmake policy warning about @rpath in MacOSX
  cmake_policy(SET CMP0042 NEW)

  set(CMAKE_MACOSX_RPATH TRUE) # initialize the MACOSX_RPATH property on all targets
  set(CMAKE_SKIP_BUILD_RPATH  FALSE) # don't skip the full RPATH for the build tree
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) # when building, use the install RPATH already
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # LC_RPATH for CUDA and OpenCV etc written into executable
endif(APPLE)

find_package(CUDA 7.0)

if(CUDA_FOUND)
  set(CUDA_SEPARABLE_COMPILATION ON)
  include_directories(${CUDA_INCLUDE_DIRS})
  add_definitions("-DWITH_CUDA")
  set(CUDA_NVCC_FLAGS_DEBUG   "${CUDA_NVCC_FLAGS_DEBUG};-DTHIS_IS_NVCC_DEBUG_MODE;-G;-g")
  set(CUDA_NVCC_FLAGS_RELEASE "${CUDA_NVCC_FLAGS_RELEASE};-DTHIS_IS_NVCC_RELEASE_MODE;-O3")
  set(CUDA_NVCC_FLAGS         "${CUDA_NVCC_FLAGS};-gencode=arch=compute_52,code=sm_52;-gencode=arch=compute_52,code=compute_52;--default-stream;per-thread")
  # DOES NOT WORK UNTIL CUDA 7.5:
  # set(CUDA_NVCC_FLAGS         "${CUDA_NVCC_FLAGS};-Xptxas;--warn-on-local-memory-usage")
  # set(CUDA_NVCC_FLAGS         "${CUDA_NVCC_FLAGS};-Xptxas;--warn-on-spills")
else(CUDA_FOUND)
    message( FATAL_ERROR "Failed to find CUDA" )
endif(CUDA_FOUND)

cuda_find_library_local_first(CUDA_CUDADEVRT_LIBRARY cudadevrt "\"cudadevrt\" library")

set( PopAssist_CPP
	src/assist.cpp
	src/device_prop.cpp
	src/debug_macros.cpp )

set( PopAssist_H
	src/assist.h
	src/clamp.h
	src/device_prop.h
	src/debug_macros.h )

add_library(popassist STATIC ${PopAssist_CPP})

install(TARGETS popassist
        LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(FILES ${PopAssist_H}
        DESTINATION include/popassist)

