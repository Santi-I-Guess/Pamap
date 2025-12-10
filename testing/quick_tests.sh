#!/usr/bin/env bash

set -o errexit -o nounset
cd "$(dirname "${0}")" || exit

   debug_dir="logs"
examples_dir="inputs"
  executable="../pamap"

log_name() {
    echo "${1}" | sed "s/${examples_dir}\/input_\(.*\)/${debug_dir}\/log_\1/"
}

if [[ ! -d ${examples_dir} ]]; then
    printf "could not find %s\n" "${examples_dir}"
    exit
elif [[ ! -d ${debug_dir} ]]; then
    printf "could not find %s\n" "${debug_dir}"
    exit
fi

# 
if [[ $# -ne 1 ]]; then
    printf "usage: quick_tests.sh <regenerate|batch>\n"
    exit
fi

case ${1} in 
    "regenerate")
        # generate gibberish examples
        rm -f "${examples_dir}"/*
        python3 "generate_valid_inputs.py" ${examples_dir}
        ;;
    "batch")
        # run valgrind on gibberish examples, store log in debug_logs
        rm -f "${debug_dir}"/*
        for example_file in $(find "${examples_dir}" -type f | sort); do
            debug_file=$(log_name "${example_file}")
            echo "${example_file} -> ${debug_file}"
            valgrind -s --log-file="${debug_file}" "${executable}" "${example_file}"
            # remove process number from log
            sed -i "s/==[0-9]\+== //" "${debug_file}"
        done
        ;;
    *)
        printf "chose an undefined option\n"
        ;;
esac
