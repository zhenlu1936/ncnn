cd ../build
cmake --build . -j 8
cmake --build . --target install

cd ..
cp -r benchmark/* build/benchmark
cp -r zhenlu/squeezenet/* build/examples