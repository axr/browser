#!/bin/bash
set -e

# Clone all submodules
git submodule update --init --recursive

# Clone dependent repositories
git clone --recursive https://github.com/axr/core.git ../core
git clone https://github.com/axr/common.git ../common
git clone https://github.com/axr/framework.git ../framework

# Install dependencies
../core/pre-configure.sh

# Make build directories
mkdir ../build

args="-D CMAKE_BUILD_TYPE=Release -D AXR_ALL_WARNINGS=ON"

# Configure using CMake
cd ../build
cmake $args ../browser
