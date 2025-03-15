#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class BufferBase;
class TextureBase;
class RenderPassBase;
class PipelineBase;

class RendererBase {
  public:
    virtual ~RendererBase() {}

    virtual void SetSurface(void* surface) = 0;

    virtual void Present(TextureBase* texture) = 0;

    // Buffer
    virtual BufferBase* CreateBuffer(const BufferDescriptor& descriptor) = 0;
    virtual void BindVertexBuffer(BufferBase* buffer, u32 index) = 0;

    // Texture
    virtual TextureBase* CreateTexture(const TextureDescriptor& descriptor) = 0;
    virtual void UploadTexture(TextureBase* texture, void* data) = 0;

    // Command buffer
    virtual void BeginCommandBuffer() = 0;
    virtual void EndCommandBuffer() = 0;

    // Render pass
    virtual RenderPassBase*
    CreateRenderPass(const RenderPassDescriptor& descriptor) = 0;
    virtual void BindRenderPass(const RenderPassBase* render_pass) = 0;

    // Shader
    virtual ShaderBase* CreateShader(const ShaderDescriptor& descriptor) = 0;

    // Pipeline
    virtual PipelineBase*
    CreatePipeline(const PipelineDescriptor& descriptor) = 0;
    virtual void BindPipeline(const PipelineBase* pipeline) = 0;

    // Clear
    virtual void ClearColor(u32 render_target_id, u32 layer, u8 mask,
                            const u32 color[4]) = 0;

    // Draw
    virtual void Draw(const u32 start, const u32 count) = 0;

  protected:
    // State getters
    // TODO

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
