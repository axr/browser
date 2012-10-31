#!/bin/bash
set -e

# Clone all submodules
git submodule update --init --recursive

# Make build directories
mkdir ../build

args="-D CMAKE_BUILD_TYPE=Release -D AXR_ALL_WARNINGS=ON"

# Configure using CMake
cd ../build
cmake $args ../browser
