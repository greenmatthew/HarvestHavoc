# @file    CMakeLists.txt
# @author  Matthew Green
# @date    2023-12-27 23:33:47
# 
# @section LICENSE
# 
# Copyright (c) 2023 Matthew Green - All rights reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

cmake_minimum_required(VERSION 3.10)

project(HarvestHavoc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HarvestHavoc)

# Add subdirectories
add_subdirectory(libs/velecs)

# Define source files
set(HARVEST_HAVOC_SOURCES
    src/Main.cpp
)

set(HARVEST_HAVOC_HEADERS
    # include/SampleHeader.hpp
)





# Create executable
add_executable(HarvestHavoc ${HARVEST_HAVOC_SOURCES} ${HARVEST_HAVOC_HEADERS} src/version.rc)

# Include directories
target_include_directories(HarvestHavoc PRIVATE include)

# Set output directory
set_target_properties(HarvestHavoc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Link libraries
target_link_libraries(HarvestHavoc
    PRIVATE velecs-engine
)

include(libs/velecs/cmake/ProcessAssets.cmake)
setup_assets(HarvestHavoc)