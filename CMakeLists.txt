cmake_minimum_required(VERSION 3.16)
project(carrot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_VITA "Build for PS Vita" OFF)
option(BUILD_LINUX "Build for Linux" ON)
option(BUILD_MAC "Build for DARWIN" OFF)
option(BUILD_WIN "Build for MicroShaft WindBlows" OFF)

include_directories(head)

file(GLOB_RECURSE NOARCH_SRC src/multiplatform/*.cpp)

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
  message(STATUS "Building for PC (Linux)")

  find_package(SDL2 REQUIRED)
  find_package(CURL REQUIRED)

  file(GLOB_RECURSE PLATFORM_SRC src/linux/*.cpp)

  add_executable(${PROJECT_NAME}
    ${NOARCH_SRC}
    ${PLATFORM_SRC}
  )

  target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
  )

  target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2_image
    CURL::libcurl
  )

endif()
