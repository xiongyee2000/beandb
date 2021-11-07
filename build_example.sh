#!/bin/bash

set -e

script_dir=$(cd `dirname $0`; pwd)
build_dir="${script_dir}/build_examples"
source_dir="${script_dir}/examples/src"

RELEASE_BUILD=0
CMAKE_OPTIONS=""


if [ -d $source_dir ]; then
    echo ===== start building =====

    if [ -d "${build_dir}" ]; then
        rm -rf ${build_dir}/*
    else
        mkdir -p ${build_dir}
    fi    

    if [[ $RELEASE_BUILD -eq 1 ]]; then
        CMAKE_OPTIONS=$CMAKE_OPTIONS:-DCMAKE_BUILD_TYPE=Release
    else
        CMAKE_OPTIONS=$CMAKE_OPTIONS:-DCMAKE_BUILD_TYPE=Debug
    fi

    cd ${build_dir}
    cmake $CMAKE_OPTIONS $source_dir \
    && make -j
    cd ..
    echo ===== done =====
else
    echo [ERROR] src directory not found!
    exit -1
fi
