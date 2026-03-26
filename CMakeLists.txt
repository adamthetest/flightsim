cmake_minimum_required(VERSION 3.20)
project(FlightSim VERSION 0.1.0 LANGUAGES CXX)

# Allow fetched deps that still declare cmake_minimum_required < 3.5
# (e.g. Raylib 5.0) to configure under CMake 4.x without errors.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Steam Deck optimization flag
option(STEAMDECK_BUILD "Enable Steam Deck (znver2) CPU optimizations" OFF)
if(STEAMDECK_BUILD)
    add_compile_options(-march=znver2 -mtune=znver2)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
endif()

include(cmake/FetchDependencies.cmake)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(flightsim ${SOURCES})

target_include_directories(flightsim PRIVATE src)
target_link_libraries(flightsim PRIVATE raylib)

if(UNIX AND NOT APPLE)
    target_link_libraries(flightsim PRIVATE m dl pthread)
endif()

# Copy assets to build directory on every build (ALL = always runs)
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Syncing assets to build directory"
)
add_dependencies(flightsim copy_assets)

# `cmake --build build-steamdeck --target deploy` — build + rsync to Deck
add_custom_target(deploy
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/deploy_deck.sh
    DEPENDS flightsim
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Deploying to Steam Deck"
)

# `cmake --build build-steamdeck --target deploy-run` — deploy + launch on Deck
add_custom_target(deploy-run
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/deploy_deck.sh -r
    DEPENDS flightsim
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Deploying and launching on Steam Deck"
)
