cmake_minimum_required(VERSION 3.16)
project(carrot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED "Builds from the shared usagi core." ON)
option(BUILD_LINUX "Build for Linux." ON)

include_directories("../usagi/head")

if(BUILD_LINUX)
  if(BUILD_SHARED)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
  endif()
endif()

if(BUILD_SHARED)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_BUILD_RPATH "$ORIGIN")
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

find_package(PkgConfig REQUIRED)
find_library(NEKKO NAMES nekko PATHS "${CMAKE_CURRENT_BINARY_DIR}") # you must drop libnekko.a/libnekko.so into the build dir for this to work

if (BUILD_LINUX)
  pkg_check_modules(READLINE REQUIRED readline)
  file(GLOB_RECURSE PLATFORM_SRC src/linux/*.cpp)
endif()

add_executable(${PROJECT_NAME}
  ${PLATFORM_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${READLINE_INCLUDE_DIRS}
)

if (BUILD_LINUX)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${READLINE_LIBRARIES}
    ${NEKKO}
  )

  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_LINUX)
endif()
