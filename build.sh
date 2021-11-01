#!/bin/bash

set -e

#VERSION=0.1.0
VERSION=`cat src/version.h | grep "BEAN_VERSION" |  awk '{print $3}'`
VERSION=${VERSION//\"/}

#TIME=`date "+%Y%m%d-%H%M%S"`
X64_SDK_NAME="beandb-$VERSION-x64"

script_dir=$(cd `dirname $0`; pwd)
source_dir="${script_dir}/src"
build_dir="${script_dir}/build"

if [ ! -d "${source_dir}" ]; then
  echo [ERROR] src directory in not found!
  exit 1
fi

CROSS_BUILD=0
RELEASE_BUILD=1
REBUILD=1
CMAKE_OPTIONS=""

# --- useage ---
function usage()
{
cat <<- EOT

    Usage: $0 [options] [--]

    Options:
        -c|cross         Cross Build
        -d|debug         Debug Build
        -h|help          Display this message
        -r|release       Release Build
 
    Example:
        ./build.sh -c
EOT
}

#Note: Incremental build not yet supported
#        -i|inc           Incremental Build

# --- end of function ---

#---------------------------------#
#  Handle command line arguments  #
#---------------------------------#
while getopts "cdhr" opt
do
    case $opt in

        c|cross)
            CROSS_BUILD=1
            ;;
        d|debug)
            RELEASE_BUILD=0
            ;;
        h|help)
            usage
            exit 0
            ;;
#        i|inc)
#           REBUILD=0
#           ;;
        r|release)
            RELEASE_BUILD=1
            ;;
        \?)
            usage
            exit 1
            ;;

    esac    # --- end of case ---
done

if [[ $RELEASE_BUILD -eq 1 ]]; then
    CMAKE_OPTIONS=$CMAKE_OPTIONS:-DCMAKE_BUILD_TYPE=Release
else
    CMAKE_OPTIONS=$CMAKE_OPTIONS:-DCMAKE_BUILD_TYPE=Debug
fi

if [ $CROSS_BUILD -eq 1 ]; then
    echo "Cross build not yet supported!"
    exit 0

    #dist_dir="$script_dir/dist/arm"
else
    dist_dir="$script_dir/dist/x64"
fi

#clean last time build result
if [ $REBUILD -eq 1 ]; then
    rm -rf $dist_dir
    mkdir $dist_dir -p 
fi

echo "================================================================"
echo "Building Bean..."
echo "================================================================"
rm -rf $build_dir \
&& mkdir $build_dir -p \
&& cd $build_dir \
&& cmake $CMAKE_OPTIONS $source_dir \
&& make -j 4 | tee build.log 
echo "================================================================"
echo "Building done."
echo "================================================================"
echo " "

if [[ $RELEASE_BUILD -eq 1 ]]; then
strip $build_dir/lib*.so
fi

echo "================================================================"
echo "Generating SDK package..."
echo "================================================================"
cd $build_dir
rm -rf $dist_dir \
&& mkdir $dist_dir -p \
&& mkdir $dist_dir/include -p \
&& mkdir $dist_dir/lib -p \
&& mkdir $dist_dir/dependencies/libs -p \
&& mkdir $dist_dir/bin -p  \
&& cp -rf ${source_dir}/* "$dist_dir/include" \
&& find "$dist_dir/include" -type f ! -name "*.h" -exec rm -rf {} \; \
&& cp -rf $build_dir/lib*.so $dist_dir/lib \
&& cp -rf $script_dir/dependencies/libs/x64 $dist_dir/dependencies/libs \
&& cd $dist_dir/.. \
&& tar zcvf ${X64_SDK_NAME}.gz  ./x64 \
&& mv ${X64_SDK_NAME}.gz $dist_dir
echo "================================================================"
echo "SDK package generated."
echo "================================================================"
echo " "

cd $script_dir
