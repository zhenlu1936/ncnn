#!/bin/bash
# zhenlu: notice that when running on gpu, the output is Apple Mx, not something specifically related to vulkan
# like "NCNN_LOGE("[%u %s]  queueC=%u[%u]  queueT=%u[%u]", i, gpu_info.device_name()," and so on...
# zhenlu: finally get vkcube replayed... the error seems to come from legacy env settings?
# zhenlu: validation errors, why? the validation layer has been shut down by ncnn.

cd squeezenet

# VK_LOADER_DEBUG=all \
VK_ADD_LAYER_PATH="$HOME/coding/gfxreconstruct/build/layer" \
VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_gfxreconstruct \
GFXRECON_CAPTURE_FILE="$PWD/output/records.gfxr" \
GFXRECON_CAPTURE_FILE_TIMESTAMP=false \
GFXRECON_LOG_LEVEL=debug \
GFXRECON_LOG_FILE="$PWD/output/record.log" \
$@

# VK_ADD_LAYER_PATH="$HOME/coding/gfxreconstruct/build/layer" \
# VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_gfxreconstruct \
# GFXRECON_CAPTURE_MODE=global \
# GFXRECON_CAPTURE_DESTINATION=file \
# GFXRECON_CAPTURE_FILE="$PWD/output/records.gfxr" \
# GFXRECON_CAPTURE_FILE_OVERWRITE=1 \
# GFXRECON_LOG_LEVEL=info \
# GFXRECON_LOG_FILE="$PWD/output/gfxrecon.log" \
# GFXRECON_CAPTURE_FILE_TIMESTAMP=false \
# $@

# $HOME/coding/gfxreconstruct/build/tools/capture-vulkan/gfxrecon-capture-vulkan.py \
#      --no-file-timestamp -o "$PWD/output/records.gfxr" \
#      --log-file "$PWD/output/gfxrecon.log" \
#      --log-level info \
#      $@