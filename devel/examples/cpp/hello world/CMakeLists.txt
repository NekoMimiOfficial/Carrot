cmake_minimum_required(VERSION 3.20)
project(carrot_sdl VERSION 1.0.0)

message(STATUS "Linux x86_64 glibc module.")

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(".")

add_library(${PROJECT_NAME} SHARED mod.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  POSITION_INDEPENDENT_CODE ON
)

target_link_options(${PROJECT_NAME} PRIVATE
  "-static-libgcc"
  "-static-libstdc++"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  SDL2
  SDL2_image
)

