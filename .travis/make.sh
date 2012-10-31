#!/bin/bash
set -e

# Build all variants
cd ..

make -C build all package package_source
