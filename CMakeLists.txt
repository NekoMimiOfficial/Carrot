cmake_minimum_required(VERSION 3.16)
project(carrot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_VITA "Build for PS Vita" OFF)
option(BUILD_LINUX "Build for Linux" ON)
option(BUILD_MAC "Build for DARWIN" OFF)
option(BUILD_WIN "Build for MicroShaft WindBlows" OFF)
option(BUILD_CLI "Ninjin CLI only" OFF)

include_directories(head)
include_directories("src/multiplatform")

file(GLOB_RECURSE NOARCH_SRC src/multiplatform/*.cpp)

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

if(BUILD_VITA)
  message(STATUS "Building for PS Vita")

  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
      set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
      message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
  endif()

  include("${VITASDK}/share/vita.cmake" REQUIRED)

  file(GLOB_RECURSE PLATFORM_SRC src/vita/*.cpp)

  find_package(SDL2 REQUIRED)

  set(VITA_APP_NAME "Carrot Loader")
  set(VITA_TITLEID  "NEMIKO003")
  set(VITA_VERSION  "01.00")

  set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

  add_executable(${PROJECT_NAME}
    ${NOARCH_SRC}
    ${PLATFORM_SRC}
  )

  target_link_libraries(${PROJECT_NAME}
    SDL2_mixer
    SDL2_image
    SDL2_ttf
    SDL2
    vorbisfile
    vorbis
    ogg
    opus
    freetype
    png
    bz2
    SceAudio_stub
    SceDisplay_stub
    SceCtrl_stub
    SceNet_stub
    SceNetCtl_stub
    pthread
    ${SDL2_LIBRARIES}
    z
  )

  vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
  vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE sce_sys/icon0.png vita/sce_sys/icon0.png
    FILE sce_sys/livearea/contents/bg.png vita/sce_sys/livearea/contents/bg.png
    FILE sce_sys/livearea/contents/startup.png vita/sce_sys/livearea/contents/startup.png
    FILE sce_sys/livearea/contents/template.xml vita/sce_sys/livearea/contents/template.xml
    FILE example.nin data/app.nin
  )

elseif (BUILD_LINUX)
    find_package(SDL2 REQUIRED)
    find_package(CURL REQUIRED)
    find_package(Threads REQUIRED)
    
    file(GLOB_RECURSE PLATFORM_SRC src/linux/*.cpp)

  if (BUILD_CLI)
    message(STATUS "Building for PC (Linux) (CLI only)")
    file(GLOB_RECURSE RUNTIME_SRC src/cli/*.cpp)

    add_executable(${PROJECT_NAME}-cli
      ${NOARCH_SRC}
      ${PLATFORM_SRC}
      ${RUNTIME_SRC}
      "${CMAKE_CURRENT_BINARY_DIR}/meta.cpp"
    )

    pkg_check_modules(READLINE REQUIRED readline)

    target_include_directories(${PROJECT_NAME}-cli PRIVATE
      ${SDL2_INCLUDE_DIRS}
      ${READLINE_INCLUDE_DIRS}
      ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(${PROJECT_NAME}-cli
      dl
      SDL2
      SDL2_image
      CURL::libcurl
      Threads::Threads
      ${READLINE_LIBRARIES}
    )
  endif()

endif()
