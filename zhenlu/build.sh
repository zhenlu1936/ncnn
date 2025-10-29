# use clang otherwise fp16fml will fail
# use clangd for parsing compile_commands.json

cd ..

mkdir -p build
cd build

cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DVulkan_LIBRARY=$VULKAN_SDK/lib/libMoltenVK.dylib \
    -DNCNN_VULKAN=ON \
    -DNCNN_BUILD_EXAMPLES=ON \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    ..

cmake --build . -j 8
cmake --build . --target install