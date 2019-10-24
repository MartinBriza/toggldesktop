#!/bin/bash

set -e

PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

[[ $PROBABLY_TAG =~ v[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]* ]] && echo $PROBABLY_TAG
