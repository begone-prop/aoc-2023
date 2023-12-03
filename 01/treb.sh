#!/bin/bash

set -e

awk -F '' '{gsub(/[a-z]/, ""); print $1($NF==""?$1:$NF)}' input  \
    | paste -sd '+' \
    | bc \
    | xargs -r echo "Part 1:"
