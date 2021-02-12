#!/bin/bash

name=$0
dir=$(dirname ${name})
shell_path=$(pwd)/${0}
dir_path=$(dirname ${shell_path})
cd ${dir_path}
abs_path=$(pwd)
abs_output_path=${abs_path}/unit_test_output
abs_unit_binary_path=${abs_path}/build_ut/bin
gcov_abs_output_path=${abs_output_path}/coverage
gcov_abs_output_report_path=${gcov_abs_output_path}/coverage_report
gcov_abs_output_info_path=${gcov_abs_output_path}/coverage_info
gcov_abs_input_path=${abs_path}/build_ut

rm -rf ${abs_path}/unit_test_output

#the following part is used for generate gcoverage report
function collect_gcov()
{
  gcov_output_dir=$1
  gcov_input_dir=$2

  for gf in `find ${gcov_input_dir} -name "*.gcda" | grep "/org/jinsha/bean"`
  do
    file_dir=${gf%/*}
    gfname=`basename $gf`
    gf=${gf%.*}
    if [ -f ${gf}.gcda ]; then
      cp ${gf}.gcda ${gcov_output_dir}
      cp ${gf}.gcno ${gcov_output_dir}
    fi
  done
}


if test -n "$(find ${gcov_abs_input_path} -name "*.gcda")"; then
  rm -rf ${abs_output_path}
  mkdir -p ${gcov_abs_output_path}
  mkdir -p ${gcov_abs_output_report_path}
  mkdir -p ${gcov_abs_output_info_path}

  collect_gcov ${gcov_abs_output_info_path} ${gcov_abs_input_path}

  lcov -c -d  ${gcov_abs_output_info_path} -o ${gcov_abs_output_info_path}/raw_coverage.info --no-recursion #--no-external
  lcov --remove ${gcov_abs_output_info_path}/raw_coverage.info "/usr/include/*" "/usr/local/include/*" "*/googletest/*" "*/unit_test/*" "*/3rdParty/*" "*/include/*" -o ${gcov_abs_output_info_path}/coverage.info 
  genhtml ${gcov_abs_output_info_path}/coverage.info -o ${gcov_abs_output_report_path}

  rm -rf $gcov_abs_output_tmp_path

else
  echo "No .gcda files found. Run run_ut.sh to generate the .gcda files if it has never been run before."
  exit -1
fi

