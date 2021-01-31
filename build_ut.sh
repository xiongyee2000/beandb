#!/bin/bash

set -e

script_dir=$(cd `dirname $0`; pwd)
build_dir="${script_dir}/build_ut"
source_dir="${script_dir}/unit_test/src"

RELEASE_BUILD=0
CMAKE_OPTIONS=""

# --- useage ---
function usage()
{
cat <<- EOT

    Usage: $0 [options] [--]

    Options:
        -r|release       Release Build
        -d|debug         Debug Build
        -h|help          Display this message

    Example:
        ./unit_test_build.sh -d
EOT
}
# --- end of function ---

#---------------------------------#
#  Handle command line arguments  #
#---------------------------------#
while getopts "rdh" opt
do
    case $opt in

        r|release)
            RELEASE_BUILD=1
            ;;

        d|debug)
            RELEASE_BUILD=0
            ;;

        h|help)
            usage
            exit 0
            ;;

        \?)
            usage
            exit 1
            ;;

    esac    # --- end of case ---
done

if [ -d "unit_test" ]; then
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
    && make -j 4
    cd ..
    echo ===== done =====
else
    echo [ERROR] src directory not found!
    exit -1
fi
