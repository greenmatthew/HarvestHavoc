cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0079 NEW)

project(HarvestHavoc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FINAL_OUTPUT_BASE_DIR "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Final output directory for executables and resources")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HarvestHavoc)

# Additional libs
add_subdirectory(libs)

# Define the HarvestHavoc executable
file(GLOB_RECURSE HARVEST_HAVOC_HEADERS "include/*.h")
file(GLOB_RECURSE HARVEST_HAVOC_SOURCES "src/*.cpp")

add_executable(HarvestHavoc ${HARVEST_HAVOC_SOURCES} ${HARVEST_HAVOC_HEADERS})

set_target_properties(HarvestHavoc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${FINAL_OUTPUT_BASE_DIR}/$<CONFIG>"
)

target_include_directories(HarvestHavoc PUBLIC include)

# Link against velecs library
target_link_libraries(HarvestHavoc PRIVATE velecs)

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
