#!/bin/bash

set -eo pipefail

repo=$(git rev-parse --show-toplevel)

find "$repo" -maxdepth 2 -regextype 'egrep' -iregex '.*/[0-9]+/[a-z_]+\.md' -not -name 'README.md' -printf '%p %h/README.md\n' \
    | xargs -rn 2 ln -srv 2>&1 \
    | grep -Fv 'File exists'
