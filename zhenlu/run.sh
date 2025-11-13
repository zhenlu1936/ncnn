#!/bin/bash
# zhenlu: notice that when running on gpu, the output is Apple Mx, not something specifically related to vulkan
# like "NCNN_LOGE("[%u %s]  queueC=%u[%u]  queueT=%u[%u]", i, gpu_info.device_name()," and so on...

cd squeezenet

# VK_LOADER_DEBUG=all \
# VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_gfxreconstruct \
# GFXRECON_CAPTURE_FILE="$PWD/output/records.gfxr" \
# GFXRECON_CAPTURE_FILE_TIMESTAMP=false \
# GFXRECON_LOG_LEVEL=info \
# GFXRECON_LOG_FILE="$PWD/output/record.log" \
../../build/examples/squeezenet dog.jpg