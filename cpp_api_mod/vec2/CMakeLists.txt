cmake_minimum_required(VERSION 3.16)
project(carrot_vec2 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Linux x86_64 glibc module.")

find_package(SDL2 REQUIRED)

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
