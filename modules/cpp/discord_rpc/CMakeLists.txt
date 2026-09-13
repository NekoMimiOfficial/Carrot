cmake_minimum_required(VERSION 3.16)
project(carrot_discord_rpc VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_language(C)

find_package(Threads REQUIRED)

set(DISCORD_RPC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/discord-rpc/lib")

set(_old_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_library(DISCORD_RPC_LIB
    NAMES discord-rpc
    PATHS "${DISCORD_RPC_DIR}"
    NO_DEFAULT_PATH
)
set(CMAKE_FIND_LIBRARY_SUFFIXES ${_old_suffixes})

if(NOT DISCORD_RPC_LIB)
    message(FATAL_ERROR "Could not find libdiscord-rpc.a in ${DISCORD_RPC_DIR}... did you drop the static library there?")
endif()

add_library(${PROJECT_NAME} SHARED
    mod.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "."
    "${DISCORD_RPC_DIR}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${DISCORD_RPC_LIB}
    Threads::Threads
)

target_link_options(${PROJECT_NAME} PRIVATE
    "-static-libgcc"
    "-static-libstdc++"
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/carrot/CAPI"
)
