#!/usr/bin/env bash

if [ $# -eq 0 ]
then
    echo "Requires file to format!"
	exit 1
fi

if [ -z "$1" ]
then
    echo "Requires non empty file name!"
	exit 1
fi

tool_name=$(basename ${1%.*})
program_name=$(basename $(grep "Command:" $1 | cut -d" " -f3))
valgrind_body=$(tail -n +5 $1)


cat << EOF > output.md
# Program Analysis

Analyzed program: $program_name

---

## $tool_name

$valgrind_body
EOF