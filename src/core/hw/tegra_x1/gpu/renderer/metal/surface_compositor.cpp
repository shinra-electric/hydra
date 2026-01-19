#include "core/hw/tegra_x1/gpu/renderer/metal/surface_compositor.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/blit_pipeline_cache.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/texture.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

SurfaceCompositor::SurfaceCompositor(CA::MetalDrawable* drawable_,
                                     MTL::CommandQueue* command_queue)
    : drawable{drawable_} {
    TMP_AUTORELEASE_POOL_BEGIN();

    // Command buffer
    command_buffer = command_queue->commandBuffer()->retain();

    // Encoder
    NS_STACK_SCOPED auto render_pass_descriptor =
        MTL::RenderPassDescriptor::alloc()->init();
    auto color_attachment =
        render_pass_descriptor->colorAttachments()->object(0);
    color_attachment->setTexture(drawable->texture());
    color_attachment->setLoadAction(MTL::LoadActionClear);
    color_attachment->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
    color_attachment->setStoreAction(MTL::StoreActionStore);

    encoder =
        command_buffer->renderCommandEncoder(render_pass_descriptor)->retain();

    TMP_AUTORELEASE_POOL_END();
}

SurfaceCompositor::~SurfaceCompositor() {
    // Encoder
    encoder->endEncoding();
    encoder->release();

    // Command buffer
    command_buffer->presentDrawable(drawable);
    command_buffer->commit();
    command_buffer->release();
}

void SurfaceCompositor::DrawTexture(const TextureBase* texture,
                                    const FloatRect2D src_rect,
                                    const FloatRect2D dst_rect,
                                    bool transparent, f32 opacity) {
    auto texture_impl = static_cast<const Texture*>(texture);

    // Draw
    encoder->setRenderPipelineState(
        METAL_RENDERER_INSTANCE.GetBlitPipelineCache()->Find(
            {drawable->texture()->pixelFormat(), transparent}));
    encoder->setViewport(MTL::Viewport{
        (f64)dst_rect.origin.x(), (f64)dst_rect.origin.y(),
        (f64)dst_rect.size.x(), (f64)dst_rect.size.y(), 0.0, 1.0});

    u32 zero = 0;
    encoder->setVertexBytes(&zero, sizeof(zero), 0);

    // Src rect
    const auto src_width = texture->GetDescriptor().width;
    const auto src_height = texture->GetDescriptor().height;
    BlitParams params = {
        .src_offset = {src_rect.origin.x() / src_width,
                       src_rect.origin.y() / src_height},
        .src_scale = {src_rect.size.x() / src_width,
                      src_rect.size.y() / src_height},
        .opacity = opacity,
    };

    encoder->setFragmentBytes(&params, sizeof(params), 0);
    encoder->setFragmentTexture(texture_impl->GetTexture(), NS::UInteger(0));
    encoder->setFragmentSamplerState(METAL_RENDERER_INSTANCE.GetLinearSampler(),
                                     NS::UInteger(0));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0),
                            NS::UInteger(3));
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
