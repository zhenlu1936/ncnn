#!/bin/bash

rm -r squeezenet/output
mkdir -p squeezenet/output

./record.sh ../../build/examples/squeezenet dog.jpg
# ./record.sh $HOME/VulkanSDK/1.4.328.1/Applications/vkcube.app/Contents/MacOS/vkcube --c 20
./convert.sh --format json squeezenet/output/records.gfxr --output squeezenet/output/trace.json
./replay.sh --log-level debug --log-file squeezenet/output/replay.log squeezenet/output/records.gfxr 