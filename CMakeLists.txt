cmake_minimum_required(VERSION 3.0.0)
project(eyeDetection VERSION 0.1.0)

include(CTest)
enable_testing()



#add_subdirectory(../dlib-19.23 dlib_build)
include(../../dlib-19.23/dlib/cmake)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(eyeDetection main.cpp  bmp.cpp HibridMedianfilter.cpp array.cpp system_metrics.cpp memcount.cpp)


find_package(dlib REQUIRED)
include_directories(${dlib_INCLUDE_DIRS})
#add_library(dlib SHARED IMPORTED)
link_directories(${dlib_LIBRARY_DIRS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} dlib )

#target_link_libraries(eyeDetection ${dlib_libs})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
