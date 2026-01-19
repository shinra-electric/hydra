#pragma once

#include "core/hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/surface_compositor.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

class SurfaceCompositor final : public ISurfaceCompositor {
  public:
    SurfaceCompositor(CA::MetalDrawable* drawable_,
                      MTL::CommandQueue* command_queue);
    ~SurfaceCompositor() override;

    void DrawTexture(const TextureBase* texture, const FloatRect2D src_rect,
                     const FloatRect2D dst_rect, bool transparent,
                     f32 opacity) override;

  private:
    CA::MetalDrawable* drawable;
    MTL::CommandBuffer* command_buffer;
    MTL::RenderCommandEncoder* encoder;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
