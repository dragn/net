cmake_minimum_required(VERSION 3.0)

project(Net)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(Utils)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -fext-numeric-literals -pthread")
endif()

set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -pthread")

add_library(Net src/net.hpp src/net.cpp)
if(WIN32)
  target_link_libraries(Net Ws2_32.lib)
endif()

include_directories(src)

add_subdirectory(examples)
