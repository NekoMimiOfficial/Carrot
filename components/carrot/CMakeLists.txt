cmake_minimum_required(VERSION 3.16)
project(carrot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_LINUX "Build for Linux." ON)

include_directories("../usagi/head")

find_library(USAGI NAMES usagi PATHS "${CMAKE_CURRENT_BINARY_DIR}") # you must drop libusagi.a into the build dir for this to work

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
    ${USAGI}
  )

  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_LINUX)
endif()
