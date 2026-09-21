cmake_minimum_required(VERSION 3.16)
project(nekko VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_DBGSYM "Enables dbgsym." ON)
option(BUILD_SHARED "Builds a shared library." ON)
option(BUILD_LINUX "Build for Linux." ON)

if(BUILD_DBGSYM)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

include_directories(head)
include_directories("src/multiplatform")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

if (BUILD_LINUX)
  include_directories("platform/linux")
  file(GLOB_RECURSE PLATFORM_SRC src/linux/*.cpp)
  file(GLOB_RECURSE PLATFORM_CORE platform/linux/*.cpp)
endif()

file(GLOB_RECURSE NOARCH_SRC src/multiplatform/*.cpp)


# Meta section
# ~~~~~~~~~~~~
string(TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M:%S")
set(COMPILE_DATE ${CURRENT_DATE})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GCC_VERSION ${CMAKE_CXX_COMPILER_VERSION})
else()
    set(GCC_VERSION "N/A (Not GCC)")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../meta.c++"
    "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
    @ONLY
)
# ~~~~~~~~~~~~


if (BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED
    ${NOARCH_SRC}
    ${PLATFORM_SRC}
    ${PLATFORM_CORE}
    ${RUNTIME_SRC}
    "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
  )
else()
  add_library(${PROJECT_NAME} STATIC
    ${NOARCH_SRC}
    ${PLATFORM_SRC}
    ${PLATFORM_CORE}
    ${RUNTIME_SRC}
    "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER ../../head/*.h
)

if (BUILD_LINUX)
  target_link_libraries(${PROJECT_NAME}
    dl
    Threads::Threads
  )

  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_LINUX)
endif()
