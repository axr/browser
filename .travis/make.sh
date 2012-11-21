#!/bin/bash
set -e

# Build all variants
cd ..

cmake --build build --config Release --target all
cmake --build build --config Release --target distribution
cmake --build build --config Release --target package_source
