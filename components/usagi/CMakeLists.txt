cmake_minimum_required(VERSION 3.16)
project(carrot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_LINUX "Build for Linux." ON)

include_directories(head)
include_directories("src/multiplatform")

find_package(SDL2 REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

if (BUILD_LINUX)
  pkg_check_modules(READLINE REQUIRED readline)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/meta.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
    @ONLY
)
# ~~~~~~~~~~~~


add_executable(${PROJECT_NAME}
  ${NOARCH_SRC}
  ${PLATFORM_SRC}
  ${PLATFORM_CORE}
  ${RUNTIME_SRC}
  "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${SDL2_INCLUDE_DIRS}
  ${READLINE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

if (BUILD_LINUX)
  target_link_libraries(${PROJECT_NAME}
    dl
    SDL2
    SDL2_image
    CURL::libcurl
    Threads::Threads
    ${READLINE_LIBRARIES}
  )

  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_LINUX)
endif()
