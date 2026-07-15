cmake_minimum_required(VERSION 3.16)
project(carrot_entropy VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local)

enable_language(C)

add_library(${PROJECT_NAME} SHARED
    mod.cpp
    entropy.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "."
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

target_link_options(${PROJECT_NAME} PRIVATE
    "-static-libgcc"
    "-static-libstdc++"
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/carrot/CAPI"
)
