#!/bin/bash

source $(dirname "$BASH_SOURCE")/kesch.sh

export G2G=1
export GTCMAKE_GT_ENABLE_BACKEND_CUDA='OFF'

export GTCMAKE_CMAKE_CXX_FLAGS='-march=haswell'
export GTCMAKE_CMAKE_CXX_FLAGS_RELEASE='-Ofast -DNDEBUG'
