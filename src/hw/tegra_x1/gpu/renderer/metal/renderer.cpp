#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/buffer.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/pipeline.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/render_pass.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/shader.hpp"
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

SINGLETON_DEFINE_GET_INSTANCE(Renderer, MetalRenderer, "Metal renderer")

Renderer::Renderer() {
    SINGLETON_SET_INSTANCE(MetalRenderer, "Metal renderer");

    // Device
    device = MTL::CreateSystemDefaultDevice();
    command_queue = device->newCommandQueue();

    // Library
    MTL::Library* library =
        CreateLibraryFromSource(device, utility_shader_source);

    // Functions
    auto vertex_fullscreen =
        library->newFunction(ToNSString("vertex_fullscreen"));
    auto fragment_texture =
        library->newFunction(ToNSString("fragment_texture"));

    // Pipeline states

    // Present pipeline
    auto pipeline_descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_descriptor->setVertexFunction(vertex_fullscreen);
    pipeline_descriptor->setFragmentFunction(fragment_texture);
    pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(
        MTL::PixelFormatBGRA8Unorm); // TODO: get from layer

    NS::Error* error;
    present_pipeline =
        device->newRenderPipelineState(pipeline_descriptor, &error);
    pipeline_descriptor->release();
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

    // Caches
    clear_color_pipeline_cache = new ClearColorPipelineCache(device);

    // Clear state
    for (u32 shader_type = 0; shader_type < usize(ShaderType::Count);
         shader_type++) {
        for (u32 i = 0; i < BUFFER_COUNT; i++)
            state.textures[shader_type][i] = nullptr;
    }

    // Release
    vertex_fullscreen->release();
    fragment_texture->release();
}

Renderer::~Renderer() {
    delete clear_color_pipeline_cache;

    linear_sampler->release();
    nearest_sampler->release();

    present_pipeline->release();
    clear_color_pipeline->release();

    command_queue->release();
    device->release();

    SINGLETON_UNSET_INSTANCE();
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

    // Command buffer
    MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();

    // Render pass
    auto render_pass_descriptor = MTL::RenderPassDescriptor::alloc()->init();
    auto color_attachment =
        render_pass_descriptor->colorAttachments()->object(0);
    color_attachment->setTexture(drawable->texture());
    color_attachment->setLoadAction(MTL::LoadActionDontCare);
    color_attachment->setStoreAction(MTL::StoreActionStore);

    auto encoder = command_buffer->renderCommandEncoder(render_pass_descriptor);
    render_pass_descriptor->release();

    // Draw
    encoder->setRenderPipelineState(present_pipeline);
    encoder->setFragmentTexture(texture_impl->GetTexture(), NS::UInteger(0));
    encoder->setFragmentSamplerState(linear_sampler, NS::UInteger(0));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0),
                            NS::UInteger(3));

    encoder->endEncoding();

    // Present
    command_buffer->presentDrawable(drawable);
    command_buffer->commit();

    // Debug
#if 0
    static u32 frames = 0;
    if (capturing) {
        if (frames >= 3)
            EndCapture();
        frames++;
    }

    static bool did_capture = false;
    if (!did_capture) {
        BeginCapture();
        did_capture = true;
    }
#endif
}

BufferBase* Renderer::CreateBuffer(const BufferDescriptor& descriptor) {
    return new Buffer(descriptor);
}

void Renderer::BindVertexBuffer(BufferBase* buffer, u32 index) {
    state.vertex_buffers[index] = static_cast<Buffer*>(buffer);
}

TextureBase* Renderer::CreateTexture(const TextureDescriptor& descriptor) {
    return new Texture(descriptor);
}

void Renderer::BeginCommandBuffer() {
    ASSERT_DEBUG(!command_buffer, MetalRenderer,
                 "Command buffer already started");
    command_buffer = command_queue->commandBuffer();
}

void Renderer::EndCommandBuffer() {
    ASSERT_DEBUG(command_buffer, MetalRenderer, "Command buffer not started");

    EndEncoding();

    command_buffer->commit();
    command_buffer->release(); // TODO: release?
    command_buffer = nullptr;
}

RenderPassBase*
Renderer::CreateRenderPass(const RenderPassDescriptor& descriptor) {
    return new RenderPass(descriptor);
}

void Renderer::BindRenderPass(const RenderPassBase* render_pass) {
    state.render_pass = static_cast<const RenderPass*>(render_pass);
}

void Renderer::ClearColor(u32 render_target_id, u32 layer, u8 mask,
                          const u32 color[4]) {
    auto encoder = GetRenderCommandEncoder();

    auto texture = static_cast<Texture*>(state.render_pass->GetDescriptor()
                                             .color_targets[render_target_id]
                                             .texture);

    SetRenderPipelineState(clear_color_pipeline_cache->Find(
        {texture->GetPixelFormat(), render_target_id, mask}));
    // TODO: set viewport and scissor
    encoder->setVertexBytes(&render_target_id, sizeof(render_target_id), 0);
    encoder->setFragmentBytes(color, sizeof(u32) * 4, 0);
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0),
                            NS::UInteger(3));
}

ShaderBase* Renderer::CreateShader(const ShaderDescriptor& descriptor) {
    return new Shader(descriptor);
}

PipelineBase* Renderer::CreatePipeline(const PipelineDescriptor& descriptor) {
    return new Pipeline(descriptor);
}

void Renderer::BindPipeline(const PipelineBase* pipeline) {
    state.pipeline = static_cast<const Pipeline*>(pipeline);
}

void Renderer::BindTexture(TextureBase* texture, ShaderType shader_type,
                           u32 index) {
    state.textures[u32(shader_type)][index] = static_cast<Texture*>(texture);
}

void Renderer::Draw(const Engines::PrimitiveType primitive_type,
                    const u32 start, const u32 count) {
    auto encoder = GetRenderCommandEncoder();

    // State
    SetRenderPipelineState();
    // TODO: viewport and scissor
    for (u32 i = 0; i < VERTEX_ARRAY_COUNT; i++)
        SetVertexBuffer(i);
    // TODO: buffers
    for (u32 shader_type = 0; shader_type < usize(ShaderType::Count);
         shader_type++) {
        for (u32 i = 0; i < TEXTURE_COUNT; i++)
            SetTexture(ShaderType(shader_type), i);
    }

    // Draw
    encoder->drawPrimitives(to_mtl_primitive_type(primitive_type),
                            NS::UInteger(start), NS::UInteger(count));
}

MTL::RenderCommandEncoder* Renderer::GetRenderCommandEncoder() {
    auto mtl_render_pass = state.render_pass->GetRenderPassDescriptor();
    if (mtl_render_pass == encoder_state.render_pass)
        return GetRenderCommandEncoderUnchecked();

    encoder_state.render_pass = mtl_render_pass;
    encoder_state.render = {};
    for (u32 shader_type = 0; shader_type < usize(ShaderType::Count);
         shader_type++) {
        for (u32 i = 0; i < BUFFER_COUNT; i++)
            encoder_state.render.buffers[shader_type][i] = nullptr;
        for (u32 i = 0; i < TEXTURE_COUNT; i++)
            encoder_state.render.textures[shader_type][i] = nullptr;
    }

    return CreateRenderCommandEncoder(mtl_render_pass);
}

MTL::RenderCommandEncoder* Renderer::CreateRenderCommandEncoder(
    MTL::RenderPassDescriptor* render_pass_descriptor) {
    ASSERT_DEBUG(command_buffer, MetalRenderer, "Command buffer not started");

    EndEncoding();

    command_encoder =
        command_buffer->renderCommandEncoder(render_pass_descriptor);
    encoder_type = EncoderType::Render;
    encoder_state.render_pass = render_pass_descriptor;

    return GetRenderCommandEncoderUnchecked();
}

MTL::BlitCommandEncoder* Renderer::GetBlitCommandEncoder() {
    if (encoder_type == EncoderType::Blit)
        return GetBlitCommandEncoderUnchecked();

    ASSERT_DEBUG(command_buffer, MetalRenderer, "Command buffer not started");

    EndEncoding();

    command_encoder = command_buffer->blitCommandEncoder();
    encoder_type = EncoderType::Blit;

    return GetBlitCommandEncoderUnchecked();
}

void Renderer::EndEncoding() {
    if (encoder_type == EncoderType::None)
        return;

    command_encoder->endEncoding();
    command_encoder->release(); // TODO: release?
    command_encoder = nullptr;
    encoder_type = EncoderType::None;

    // Reset the render pass
    encoder_state.render_pass = nullptr;
}

void Renderer::SetRenderPipelineState(MTL::RenderPipelineState* mtl_pipeline) {
    if (mtl_pipeline == encoder_state.render.pipeline)
        return;

    GetRenderCommandEncoderUnchecked()->setRenderPipelineState(mtl_pipeline);
    encoder_state.render.pipeline = mtl_pipeline;
}

void Renderer::SetRenderPipelineState() {
    SetRenderPipelineState(state.pipeline->GetPipeline());
}

void Renderer::SetBuffer(MTL::Buffer* buffer, ShaderType shader_type,
                         u32 index) {
    ASSERT_DEBUG(index < BUFFER_COUNT, MetalRenderer, "Invalid buffer index {}",
                 index);

    auto& bound_buffer =
        encoder_state.render.buffers[static_cast<u32>(shader_type)][index];
    if (buffer == bound_buffer)
        return;

    switch (shader_type) {
    case ShaderType::Vertex:
        GetRenderCommandEncoderUnchecked()->setVertexBuffer(buffer, 0, index);
        break;
    case ShaderType::Fragment:
        GetRenderCommandEncoderUnchecked()->setFragmentBuffer(buffer, 0, index);
        break;
    default:
        LOG_ERROR(MetalRenderer, "Invalid shader type {}", shader_type);
        break;
    }
    bound_buffer = buffer;
}

void Renderer::SetVertexBuffer(u32 index) {
    const auto buffer = state.vertex_buffers[index];
    if (!buffer)
        return;

    SetBuffer(buffer->GetBuffer(), ShaderType::Vertex,
              GetVertexBufferIndex(index));
}

void Renderer::SetTexture(MTL::Texture* texture, ShaderType shader_type,
                          u32 index) {
    ASSERT_DEBUG(index < TEXTURE_COUNT, MetalRenderer,
                 "Invalid texture index {}", index);

    auto& bound_texture =
        encoder_state.render.textures[static_cast<u32>(shader_type)][index];
    if (texture == bound_texture)
        return;

    switch (shader_type) {
    case ShaderType::Vertex:
        GetRenderCommandEncoderUnchecked()->setVertexTexture(texture, index);
        break;
    case ShaderType::Fragment:
        GetRenderCommandEncoderUnchecked()->setFragmentTexture(texture, index);
        break;
    default:
        LOG_ERROR(MetalRenderer, "Invalid shader type {}", shader_type);
        break;
    }
    bound_texture = texture;
}

void Renderer::SetTexture(ShaderType shader_type, u32 index) {
    const auto texture = state.textures[u32(shader_type)][index];
    if (!texture)
        return;

    SetTexture(texture->GetTexture(), shader_type, index);
}

void Renderer::BeginCapture() {
    auto capture_manager = MTL::CaptureManager::sharedCaptureManager();
    auto desc = MTL::CaptureDescriptor::alloc()->init();
    desc->setCaptureObject(device);

    // Check if a debugger with support for GPU capture is attached
    if (capture_manager->supportsDestination(
            MTL::CaptureDestinationDeveloperTools)) {
        desc->setDestination(MTL::CaptureDestinationDeveloperTools);
    } else {
        // TODO: don't hardcode the directory
        const std::string gpu_capture_dir = "/Users/samuliak/Downloads";
        if (gpu_capture_dir.empty()) {
            LOG_ERROR(
                MetalRenderer,
                "No GPU capture directory specified, cannot do a GPU capture");
            return;
        }

        // Check if the GPU trace document destination is available
        if (!capture_manager->supportsDestination(
                MTL::CaptureDestinationGPUTraceDocument)) {
            LOG_ERROR(MetalRenderer, "GPU trace document destination is not "
                                     "available, cannot do a GPU capture");
            return;
        }

        // Get current date and time as a string
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_time), "%d.%m.%Y_%H:%M:%S");
        std::string now_str = oss.str();

        std::string capture_path =
            fmt::format("{}/hydra_{}.gputrace", gpu_capture_dir, now_str);
        desc->setDestination(MTL::CaptureDestinationGPUTraceDocument);
        desc->setOutputURL(ToNSURL(capture_path));
    }

    NS::Error* error = nullptr;
    capture_manager->startCapture(desc, &error);
    if (error) {
        LOG_ERROR(MetalRenderer, "Failed to start GPU capture: {}",
                  error->localizedDescription()->utf8String());
    }

    capturing = true;
}

void Renderer::EndCapture() {
    auto captureManager = MTL::CaptureManager::sharedCaptureManager();
    captureManager->stopCapture();

    capturing = false;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
