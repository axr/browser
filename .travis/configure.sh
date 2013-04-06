#!/bin/bash
set -e

# Clone all submodules
git submodule update --init --recursive

# Clone dependent repositories
git clone https://github.com/axr/core.git ../core
git clone https://github.com/axr/common.git ../common
git clone https://github.com/axr/framework.git ../framework

# Make build directories
mkdir ../build

args="-D CMAKE_BUILD_TYPE=Release -D AXR_ALL_WARNINGS=ON"

# Configure using CMake
cd ../build
cmake $args ../browser
