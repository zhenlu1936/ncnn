#!/bin/bash

export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"

$HOME/coding/gfxreconstruct/build/tools/replay/gfxrecon-replay $@ 