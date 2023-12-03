#!/bin/bash

set -eo pipefail

repo=$(git rev-parse --show-toplevel)

find "$repo" -executable -not -path '*.git/*' -type f -print0 \
    | xargs -r0 file \
    | grep 'ELF.*executable' \
    | cut -d ':' -f 1 \
    | xargs -r rm -vf
