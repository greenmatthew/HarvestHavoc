#!/bin/bash

# Run cloc with forced language options for .DISABLED files
cloc \
--ignored=ignored.txt \
--force-lang="C++",cpp.DISABLED \
--force-lang="C/C++ Header",h.DISABLED \
CMakeLists.txt \
*.md \
Makefile \
src/ \
include/ \
libs/CMakeLists.txt \
libs/velecs/CMakeLists.txt \
libs/velecs/src \
libs/velecs/include \
libs/velecs/libs/CMakeLists.txt \
libs/velecs/assets/CMakeLists.txt \
libs/velecs/assets/shaders/ \
libs/velecs/assets/shaders/CMakeLists.txt \
libs/velecs/assets/meshes/CMakeLists.txt

# Check if ignored.txt was created and is not empty
if [ -s ignored.txt ]; then
    cat ignored.txt
    rm ignored.txt
fi
