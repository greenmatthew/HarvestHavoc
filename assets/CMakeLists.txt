# @file    CMakeLists.txt
# @author  Matthew Green
# @date    2023-11-30 13:48:57
# 
# @section LICENSE
# 
# Copyright (c) 2023 Matthew Green - All rights reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

cmake_minimum_required(VERSION 3.10)

include(../libs/velecs/cmake/AssetUtility.cmake)

# Base path for assets
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
debug_message("Assets directory: ${ASSETS_DIR}")

set(DEST_DIR "${FINAL_OUTPUT_BASE_DIR}/$<CONFIG>/assets")

set(PATHS_TO_IGNORE
    "*.bak"
    "*.DISABLED"
    "*/CMakeLists.txt"
    "${ASSETS_DIR}/meshes/source/"
)
debug_message("Ignored patterns: ${PATHS_TO_IGNORE}")

# Accumulate all asset files to be copied
set(ALL_COPIED_ASSET_FILES "")
process_assets("${ASSETS_DIR}" "${DEST_DIR}" "${PATHS_TO_IGNORE}" ALL_COPIED_ASSET_FILES)

# Create a single custom target for all assets
add_custom_target(assets ALL DEPENDS ${ALL_COPIED_ASSET_FILES})
debug_message("Custom target 'assets' depends on: ${ALL_COPIED_ASSET_FILES}")
