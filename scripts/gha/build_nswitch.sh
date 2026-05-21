#!/bin/bash
mkdir -p artifacts/ || exit 1

docker run --name xash-build --rm -v "$(pwd):$(pwd)" -w "$(pwd)" devkitpro/devkita64:latest bash ./scripts/gha/build_nswitch_docker.sh || exit 1
