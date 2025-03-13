#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

#include "Foundation/NSTypes.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/render_pass.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/texture.hpp"

// TODO: define in a separate file
const std::string utility_shader_source = R"(
#include <metal_stdlib>
using namespace metal;

constant float2 vertices[3] = {
    float2(-1.0, -3.0),
    float2(-1.0,  1.0),
    float2( 3.0,  1.0)
};

struct FullscreenVertexOut {
    float4 position [[position]];
    float2 tex_coord;
};

vertex FullscreenVertexOut vertex_fullscreen(ushort vid [[vertex_id]]) {
    FullscreenVertexOut out;
    out.position = float4(vertices[vid], 0.0, 1.0);
    out.tex_coord = vertices[vid] * 0.5 + 0.5;
    out.tex_coord.y = 1.0 - out.tex_coord.y;

    return out;
}

fragment float4 fragment_texture(FullscreenVertexOut in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler samplr [[sampler(0)]]) {
    return float4(tex.sample(samplr, in.tex_coord).rgb, 1.0);
}
)";

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

static Renderer* s_instance;

Renderer& Renderer::GetInstance() { return *s_instance; }

Renderer::Renderer() {
    ASSERT(s_instance == nullptr, GPU, "Metal renderer already exists");
    s_instance = this;

    // Device
    device = MTL::CreateSystemDefaultDevice();
    command_queue = device->newCommandQueue();

    // Library
    NS::Error* error;
    MTL::Library* library =
        device->newLibrary(ToNSString(utility_shader_source), nullptr, &error);
    if (error) {
        LOG_ERROR(GPU, "Failed to create library: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
    }

    // Functions
    auto vertex_function =
        library->newFunction(ToNSString("vertex_fullscreen"));
    auto fragment_function =
        library->newFunction(ToNSString("fragment_texture"));

    // Pipeline states

    // Present pipeline
    auto present_pipeline_descriptor =
        MTL::RenderPipelineDescriptor::alloc()->init();
    present_pipeline_descriptor->setVertexFunction(vertex_function);
    present_pipeline_descriptor->setFragmentFunction(fragment_function);
    present_pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(
        MTL::PixelFormatBGRA8Unorm); // TODO: get from layer

    present_pipeline_state =
        device->newRenderPipelineState(present_pipeline_descriptor, &error);
    present_pipeline_descriptor->release();
    if (error) {
        LOG_ERROR(GPU, "Failed to create present pipeline state: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
    }

    // Sampler states

    // Nearest
    auto sampler_state_descriptor = MTL::SamplerDescriptor::alloc()->init();
    nearest_sampler = device->newSamplerState(sampler_state_descriptor);

    // Linear
    sampler_state_descriptor->setMinFilter(MTL::SamplerMinMagFilterLinear);
    sampler_state_descriptor->setMagFilter(MTL::SamplerMinMagFilterLinear);
    linear_sampler = device->newSamplerState(sampler_state_descriptor);
    sampler_state_descriptor->release();
}

Renderer::~Renderer() {
    linear_sampler->release();
    nearest_sampler->release();

    present_pipeline_state->release();

    command_queue->release();
    device->release();

    s_instance = nullptr;
}

void Renderer::SetSurface(void* surface) {
    layer = reinterpret_cast<CA::MetalLayer*>(surface);
    layer->setDevice(device);
    // TODO: set pixel format
}

void Renderer::Present(TextureBase* texture) {
    auto texture_impl = static_cast<Texture*>(texture);

    // TODO: acquire drawable earlier?
    auto drawable = layer->nextDrawable();

    // Render pass
    auto render_pass_descriptor = MTL::RenderPassDescriptor::alloc()->init();
    auto color_attachment =
        render_pass_descriptor->colorAttachments()->object(0);
    color_attachment->setTexture(drawable->texture());
    color_attachment->setLoadAction(MTL::LoadActionDontCare);
    color_attachment->setStoreAction(MTL::StoreActionStore);

    auto encoder = GetTemporaryRenderCommandEncoder(render_pass_descriptor);
    render_pass_descriptor->release();

    // Draw
    encoder->setRenderPipelineState(present_pipeline_state);
    encoder->setFragmentTexture(texture_impl->GetTexture(), NS::UInteger(0));
    encoder->setFragmentSamplerState(linear_sampler, NS::UInteger(0));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0),
                            NS::UInteger(3));

    EndEncoding();

    // Present
    command_buffer->presentDrawable(drawable);
    CommitCommandBuffer();
}

TextureBase* Renderer::CreateTexture(const TextureDescriptor& descriptor) {
    return new Texture(descriptor);
}

void Renderer::UploadTexture(TextureBase* texture, void* data) {
    auto texture_impl = static_cast<Texture*>(texture);
    auto mtl_texture = texture_impl->GetTexture();

    // TODO: do a GPU copy?
    // TODO: bytes per image
    mtl_texture->replaceRegion(
        MTL::Region{0, 0, 0, mtl_texture->width(), mtl_texture->height(), 1}, 0,
        0, data, texture_impl->GetDescriptor().stride, 0);
}

RenderPassBase*
Renderer::CreateRenderPass(const RenderPassDescriptor& descriptor) {
    return new RenderPass(descriptor);
}

MTL::CommandBuffer* Renderer::GetCommandBuffer() {
    if (!command_buffer)
        command_buffer = command_queue->commandBuffer();

    return command_buffer;
}

void Renderer::CommitCommandBuffer() {
    EndEncoding();

    command_buffer->commit();
    command_buffer->release(); // TODO: release?
    command_buffer = nullptr;
}

MTL::RenderCommandEncoder* Renderer::GetTemporaryRenderCommandEncoder(
    MTL::RenderPassDescriptor* render_pass_descriptor) {
    encoder_type = EncoderType::Render;
    command_encoder =
        GetCommandBuffer()->renderCommandEncoder(render_pass_descriptor);

    return static_cast<MTL::RenderCommandEncoder*>(command_encoder);
}

void Renderer::EndEncoding() {
    if (encoder_type == EncoderType::None)
        return;

    command_encoder->endEncoding();
    command_encoder->release(); // TODO: release?
    command_encoder = nullptr;
    encoder_type = EncoderType::None;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
