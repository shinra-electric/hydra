#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase;

struct TextureDescriptor {
    uptr ptr;
    SurfaceFormat surface_format;
    NvKind kind;
    usize width;
    usize height;
    usize block_height_log2;
    usize stride;
    // TODO: more
};

struct RenderTargetDescriptor {
    TextureBase* texture;
    bool load_action_clear;
    union {
        float color[4];
        struct {
            bool clear_depth;
            bool clear_stencil;
            float depth;
            u32 stencil;
        };
    } clear_data;
};

struct RenderPassDescriptor {
    RenderTargetDescriptor color_targets[COLOR_TARGET_COUNT];
    RenderTargetDescriptor depth_stencil_target;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
