#!/bin/bash

set -e
set -x

PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

if [[ grep -Eq "^v[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*$" <<< "$PROBABLY_TAG" ]]; then
    echo $PROBABLY_TAG
fi
