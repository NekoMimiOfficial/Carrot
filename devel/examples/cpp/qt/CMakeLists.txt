cmake_minimum_required(VERSION 3.16)
project(carrot_qt VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

enable_language(C CXX)

find_package(Threads REQUIRED)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME} SHARED
    mod.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "."
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Threads::Threads
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/carrot/CAPI"
)
