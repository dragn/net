cmake_minimum_required(VERSION 3.0)

project(Net)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(Utils)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -fext-numeric-literals")
endif()

add_library(Net src/net.hpp src/net.cpp)

include_directories(src)

add_executable(net_test net_test.cpp)
target_link_libraries(net_test Net)
