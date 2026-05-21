#!/bin/bash
export PATH=${DEVKITPRO}/tools/bin:${DEVKITPRO}/devkitA64/bin:$PATH

make -C libsolder install || exit 1

cmake -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=${DEVKITPRO}/cmake/Switch.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -D64BIT=ON \
  -DUSE_VGUI=OFF \
  -DPOLLY=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/dist" \
  -B build -S .

cmake --build build --target all
cmake --build build --target install
