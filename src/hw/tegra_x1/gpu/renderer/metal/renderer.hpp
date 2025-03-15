#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/clear_color_pipeline_cache.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/renderer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Buffer;
class RenderPass;
class Pipeline;

enum class EncoderType {
    None,
    Render,
    Compute,
    Blit,
};

struct State {
    const RenderPass* render_pass{nullptr};
    const Pipeline* pipeline{nullptr};
    const Buffer* vertex_buffers[VERTEX_ARRAY_COUNT] = {nullptr};
};

struct EncoderRenderState {
    MTL::RenderPipelineState* pipeline{nullptr};
    MTL::Buffer* buffers[usize(ShaderType::Count)][BUFFER_COUNT];
};

struct EncoderState {
    MTL::RenderPassDescriptor* render_pass{nullptr};
    EncoderRenderState render{};
};

class Renderer : public RendererBase {
  public:
    static Renderer& GetInstance();

    Renderer();
    ~Renderer() override;

    void SetSurface(void* surface) override;

    void Present(TextureBase* texture) override;

    // Buffer
    BufferBase* CreateBuffer(const BufferDescriptor& descriptor) override;
    void BindVertexBuffer(BufferBase* buffer, u32 index) override;

    // Texture
    TextureBase* CreateTexture(const TextureDescriptor& descriptor) override;
    void UploadTexture(TextureBase* texture, void* data) override;

    // Command buffer
    void BeginCommandBuffer() override;
    void EndCommandBuffer() override;

    // Render pass
    RenderPassBase*
    CreateRenderPass(const RenderPassDescriptor& descriptor) override;
    void BindRenderPass(const RenderPassBase* render_pass) override;

    // Shader
    ShaderBase* CreateShader(const ShaderDescriptor& descriptor) override;

    // Pipeline
    PipelineBase* CreatePipeline(const PipelineDescriptor& descriptor) override;
    void BindPipeline(const PipelineBase* pipeline) override;

    // Clear
    void ClearColor(u32 render_target_id, u32 layer, u8 mask,
                    const u32 color[4]) override;

    // Draw
    void Draw(const u32 start, const u32 count) override;

    // Getters
    MTL::Device* GetDevice() const { return device; }

  private:
    MTL::Device* device;
    MTL::CommandQueue* command_queue;

    CA::MetalLayer* layer;

    // Pipelines
    MTL::RenderPipelineState* present_pipeline;
    MTL::RenderPipelineState* clear_color_pipeline;

    // Samplers
    MTL::SamplerState* nearest_sampler;
    MTL::SamplerState* linear_sampler;

    // Caches
    ClearColorPipelineCache* clear_color_pipeline_cache;

    // Command buffer
    MTL::CommandBuffer* command_buffer{nullptr};
    MTL::CommandEncoder* command_encoder{nullptr};
    EncoderType encoder_type{EncoderType::None};

    // State
    State state;
    EncoderState encoder_state;

    // Debug
    bool capturing = false;

    // Helpers

    // Command encoders
    MTL::RenderCommandEncoder* GetRenderCommandEncoderUnchecked() {
        ASSERT_DEBUG(encoder_type == EncoderType::Render, MetalRenderer,
                     "Invalid encoder type");
        return static_cast<MTL::RenderCommandEncoder*>(command_encoder);
    }

    MTL::RenderCommandEncoder* GetRenderCommandEncoder();

    MTL::RenderCommandEncoder* CreateRenderCommandEncoder(
        MTL::RenderPassDescriptor* render_pass_descriptor);
    void EndEncoding();

    // Encoder state setting
    void SetRenderPipelineState(MTL::RenderPipelineState* mtl_pipeline);
    void SetRenderPipelineState();
    void SetBuffer(MTL::Buffer* buffer, ShaderType shader_type, u32 index);
    void SetVertexBuffer(u32 index);

    // Debug
    void BeginCapture();
    void EndCapture();
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
