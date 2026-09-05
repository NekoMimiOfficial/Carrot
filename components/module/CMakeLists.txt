cmake_minimum_required(VERSION 3.16)
project(carrot_module VERSION 1.0.0) # rename to your carrot_MODULENAMEHERE

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local) # linux specific sowwy :3

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

enable_language(C CXX)

# if you need find_package call it below this comment

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
    # add any libraries you want to link here
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/carrot/CAPI"
)
