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

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FINAL_OUTPUT_BASE_DIR "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Final output directory for executables and resources")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HarvestHavoc)

# Additional libs
add_subdirectory(libs)
add_subdirectory(assets)

# Define the HarvestHavoc executable
file(GLOB_RECURSE HARVEST_HAVOC_HEADERS "include/*.h")
file(GLOB_RECURSE HARVEST_HAVOC_SOURCES "src/*.cpp")

add_executable(HarvestHavoc ${HARVEST_HAVOC_SOURCES} ${HARVEST_HAVOC_HEADERS} src/version.rc)

set_target_properties(HarvestHavoc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${FINAL_OUTPUT_BASE_DIR}/$<CONFIG>"
)

target_include_directories(HarvestHavoc PUBLIC include)

# Link against velecs library
target_link_libraries(HarvestHavoc PRIVATE velecs)

add_dependencies(HarvestHavoc assets)

#########################################################################################
### Start of DLL handling ###############################################################
#########################################################################################

# Define a variable for the system directory
set(SYSTEM_DIR "C:/Windows/System32/")

add_custom_command(TARGET HarvestHavoc POST_BUILD
    COMMENT "Copying DLLs to output directory"
    # Copy common DLLs
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SYSTEM_DIR}vulkan-1.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$ENV{VULKAN_SDK}/Bin/SDL2.dll" $<TARGET_FILE_DIR:HarvestHavoc>

    # Copy Debug DLLs
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}msvcp140d.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}vcruntime140d.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}vcruntime140_1d.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}ucrtbased.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    
    # Copy Release DLLs
    COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}msvcp140.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}vcruntime140.dll" $<TARGET_FILE_DIR:HarvestHavoc>
    COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E copy_if_different "${SYSTEM_DIR}vcruntime140_1.dll" $<TARGET_FILE_DIR:HarvestHavoc>
)

#########################################################################################
### End of DLL handling #################################################################
#########################################################################################

#########################################################################################
### Start of Organize source files in IDE ###############################################
#########################################################################################

# Organize source files in IDE
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
source_group(
    TREE "${ROOT_DIR}/include"
    PREFIX "Header Files"
    FILES ${HARVEST_HAVOC_HEADERS}
)
source_group(
    TREE "${ROOT_DIR}/src"
    PREFIX "Source Files"
    FILES ${HARVEST_HAVOC_SOURCES}
)

#########################################################################################
### End of Organize source files in IDE #################################################
#########################################################################################
