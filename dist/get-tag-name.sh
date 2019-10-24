#!/bin/bash

set -x

PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

grep -Eq "^v[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*$" <<< "$PROBABLY_TAG" && echo $PROBABLY_TAG
