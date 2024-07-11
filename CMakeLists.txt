cmake_minimum_required(VERSION 3.10)
project(FarRKDOCA)
find_package(ROOT)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ROOT_INCLUDE_DIRS)

add_library(FarRKDOCA SHARED FarRKDOCA.cc)

target_link_libraries(FarRKDOCA PUBLIC ROOT::Core ROOT::Matrix ROOT::Physics)
