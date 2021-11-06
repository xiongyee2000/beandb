#!/bin/bash

script_dir=$(cd `dirname $0`; pwd)
build_dir=${script_dir}/build_ut

if [ -f "${build_dir}/beandb_ut" ]; then
  "${build_dir}/beandb_ut" --gtest_output=xml
else
  echo "bean_ut not found. Run build_ut.sh if it has not been built before."
fi