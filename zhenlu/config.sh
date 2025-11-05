#!/bin/bash
# use clang otherwise fp16fml will fail
# use clangd for parsing compile_commands.json
# use llvm@20 for compatibility with std::__1::__hash_memory

cd ..

mkdir -p build
cd build

cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DVulkan_LIBRARY=$VULKAN_SDK/lib/libMoltenVK.dylib \
    -DNCNN_VULKAN=ON \
    -DNCNN_BUILD_EXAMPLES=ON \
    -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@20/bin/clang \
    -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@20/bin/clang++ \
    ..