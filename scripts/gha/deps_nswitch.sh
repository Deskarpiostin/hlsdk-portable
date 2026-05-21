#!/bin/bash
cd "$GITHUB_WORKSPACE" || exit 1
docker pull devkitpro/devkita64:latest || exit 1
git clone https://github.com/fgsfdsfgs/libsolder.git --depth=1 || exit 1