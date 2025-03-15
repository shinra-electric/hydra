#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

#include "Foundation/NSTypes.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/pipeline.hpp"
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
    /*
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
    */
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

PipelineBase* Renderer::CreatePipeline(const PipelineDescriptor& descriptor) {
    return new Pipeline(descriptor);
}

void Renderer::BindPipeline(const PipelineBase* pipeline) {
    state.render.pipeline = static_cast<const Pipeline*>(pipeline);
}

void Renderer::ClearColor(u32 render_target_id, u32 layer, u8 mask,
                          const u32 color[4]) {
    auto encoder = GetRenderCommandEncoder();

    auto texture =
        static_cast<Texture*>(encoder_state.render_pass->GetDescriptor()
                                  .color_targets[render_target_id]
                                  .texture);

    // TODO: maybe make this return a Pipeline?
    encoder->setRenderPipelineState(clear_color_pipeline_cache->Find(
        {texture->GetPixelFormat(), render_target_id, mask}));
    encoder_state.render.pipeline = nullptr;

    // TODO: set viewport and scissor
    encoder->setVertexBytes(&render_target_id, sizeof(render_target_id), 0);
    encoder->setFragmentBytes(color, sizeof(u32) * 4, 0);
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0),
                            NS::UInteger(3));
}

void Renderer::Draw(const u32 start, const u32 count) {
    auto encoder = GetRenderCommandEncoder();

    // State
    SetRenderPipelineState();

    // Draw
    // TODO: use the actual primitive type
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(start),
                            NS::UInteger(count));
}

MTL::RenderCommandEncoder* Renderer::GetRenderCommandEncoder() {
    if (encoder_state.render_pass == state.render_pass)
        return static_cast<MTL::RenderCommandEncoder*>(command_encoder);

    encoder_state.render_pass = state.render_pass;
    encoder_state.render = {};

    return CreateRenderCommandEncoder(
        encoder_state.render_pass->GetRenderPassDescriptor());
}

MTL::RenderCommandEncoder* Renderer::CreateRenderCommandEncoder(
    MTL::RenderPassDescriptor* render_pass_descriptor) {
    ASSERT_DEBUG(command_buffer, MetalRenderer, "Command buffer not started");

    EndEncoding();

    command_encoder =
        command_buffer->renderCommandEncoder(render_pass_descriptor);
    encoder_type = EncoderType::Render;

    return static_cast<MTL::RenderCommandEncoder*>(command_encoder);
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

void Renderer::SetRenderPipelineState(const Pipeline* pipeline) {
    if (pipeline == encoder_state.render.pipeline)
        return;

    GetRenderCommandEncoder()->setRenderPipelineState(pipeline->GetPipeline());
    encoder_state.render.pipeline = pipeline;
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
