project(Examples)

find_path(EV_INCLUDE_DIR ev.h)
find_library(EV_LIBRARY ev)

if(EV_INCLUDE_DIR)
  message(STATUS "Found ev.h: ${EV_INCLUDE_DIR}")
  message(STATUS "Found libev: ${EV_LIBRARY}")
else()
  message(FATAL "Could not find libev")
endif()

file(GLOB EXAMPLES *.cpp)

foreach(EXAMPLE ${EXAMPLES})
  get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
  add_executable(${EXAMPLE_NAME} ${EXAMPLE})
  target_link_libraries(${EXAMPLE_NAME} Net ev)
endforeach()
