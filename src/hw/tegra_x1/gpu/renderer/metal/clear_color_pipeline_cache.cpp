#include "hw/tegra_x1/gpu/renderer/metal/clear_color_pipeline_cache.hpp"
#include "Metal/MTLRenderPipeline.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

ClearColorPipelineCache::ClearColorPipelineCache(MTL::Device* device_)
    : device{device_} {
    // Source
    auto shader_source = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct VertexClearColorOut {
            float4 position [[position]];
            uint layer_id [[render_target_array_index]];
        };

        constant float2 vertices[3] = {
            float2(-1.0, -3.0),
            float2(-1.0,  1.0),
            float2( 3.0,  1.0)
        };

        vertex VertexClearColorOut vertex_clear_color(ushort vid [[vertex_id]], constant uint& layer_id [[buffer(0)]]) {
            VertexClearColorOut out;
            out.position = float4(vertices[vid], 0.0, 1.0);
            out.layer_id = layer_id;

            return out;
        }
    )";

    // Library
    NS::Error* error;
    MTL::Library* library =
        device->newLibrary(ToNSString(shader_source), nullptr, &error);
    if (error) {
        LOG_ERROR(GPU, "Failed to create clear color vertex library: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
    }

    // Function
    auto vertex_clear_color =
        library->newFunction(ToNSString("vertex_clear_color"));

    // Pipeline descriptor
    pipeline_descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_descriptor->setVertexFunction(vertex_clear_color);
    pipeline_descriptor->setInputPrimitiveTopology(
        MTL::PrimitiveTopologyClassTriangle);
}

void ClearColorPipelineCache::Destroy() { pipeline_descriptor->release(); }

MTL::RenderPipelineState* ClearColorPipelineCache::Create(
    const ClearColorPipelineDescriptor& descriptor) {
    // Source
    auto shader_source = R"(
        #include <metal_stdlib>
        using namespace metal;

        // TODO: choose the correct color data type
        fragment float4 fragment_clear_color(constant float4& col [[buffer(0)]]) {
            return col;
        }
    )";

    // Library
    NS::Error* error;
    MTL::Library* library =
        device->newLibrary(ToNSString(shader_source), nullptr, &error);
    if (error) {
        LOG_ERROR(GPU, "Failed to create clear color fragment library: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
    }

    // Function
    auto fragment_clear_color =
        library->newFunction(ToNSString("fragment_clear_color"));

    // Pipeline
    pipeline_descriptor->setFragmentFunction(fragment_clear_color);
    auto color_attachment = pipeline_descriptor->colorAttachments()->object(
        descriptor.render_target_id);
    color_attachment->setPixelFormat(descriptor.pixel_format);

    MTL::ColorWriteMask mask = MTL::ColorWriteMaskNone;
    if (descriptor.mask & BIT(0))
        mask |= MTL::ColorWriteMaskRed;
    if (descriptor.mask & BIT(1))
        mask |= MTL::ColorWriteMaskGreen;
    if (descriptor.mask & BIT(2))
        mask |= MTL::ColorWriteMaskBlue;
    if (descriptor.mask & BIT(3))
        mask |= MTL::ColorWriteMaskAlpha;
    color_attachment->setWriteMask(mask);

    fragment_clear_color->release();

    auto pipeline = device->newRenderPipelineState(pipeline_descriptor, &error);
    if (error) {
        LOG_ERROR(MetalRenderer, "Failed to create clear color pipeline: {}",
                  error->localizedDescription()->utf8String());
        return nullptr;
    }

    return pipeline;
}

u64 ClearColorPipelineCache::Hash(
    const ClearColorPipelineDescriptor& descriptor) {
    // TODO: implement
    return 0;
}

void ClearColorPipelineCache::DestroyElement(
    MTL::RenderPipelineState* pipeline) {
    pipeline->release();
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
