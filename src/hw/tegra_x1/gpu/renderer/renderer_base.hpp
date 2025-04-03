#pragma once

#include "hw/tegra_x1/gpu/renderer/buffer_cache.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"
#include "hw/tegra_x1/gpu/renderer/pipeline_cache.hpp"
#include "hw/tegra_x1/gpu/renderer/render_pass_cache.hpp"
#include "hw/tegra_x1/gpu/renderer/shader_cache.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_cache.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class BufferBase;
class TextureBase;
class RenderPassBase;
class PipelineBase;

struct Info {
    bool supports_quads_primitive;

    bool IsPrimitiveSupported(Engines::PrimitiveType primitive_type) const {
        switch (primitive_type) {
        case Engines::PrimitiveType::Quads:
            return supports_quads_primitive;
        default:
            return true;
        }
    }
};

class RendererBase {
  public:
    virtual ~RendererBase() {}

    virtual void SetSurface(void* surface) = 0;

    virtual void Present(TextureBase* texture) = 0;

    // Buffer
    virtual BufferBase* CreateBuffer(const BufferDescriptor& descriptor) = 0;
    virtual BufferBase* AllocateTemporaryBuffer(const usize size) = 0;
    virtual void FreeTemporaryBuffer(BufferBase* buffer) = 0;

    // Texture
    virtual TextureBase* CreateTexture(const TextureDescriptor& descriptor) = 0;

    // Command buffer
    virtual void BeginCommandBuffer() = 0;
    virtual void EndCommandBuffer() = 0;

    // Render pass
    virtual RenderPassBase*
    CreateRenderPass(const RenderPassDescriptor& descriptor) = 0;
    virtual void BindRenderPass(const RenderPassBase* render_pass) = 0;

    // Clear
    virtual void ClearColor(u32 render_target_id, u32 layer, u8 mask,
                            const uint4 color) = 0;
    virtual void ClearDepth(u32 layer, const float value) = 0;
    virtual void ClearStencil(u32 layer, const u32 value) = 0;

    // Shader
    virtual ShaderBase* CreateShader(const ShaderDescriptor& descriptor) = 0;

    // Pipeline
    virtual PipelineBase*
    CreatePipeline(const PipelineDescriptor& descriptor) = 0;
    virtual void BindPipeline(const PipelineBase* pipeline) = 0;

    // Resource binding
    virtual void BindVertexBuffer(BufferBase* buffer, u32 index) = 0;
    virtual void BindIndexBuffer(BufferBase* index_buffer,
                                 Engines::IndexType index_type) = 0;
    virtual void BindUniformBuffer(BufferBase* buffer, ShaderType shader_type,
                                   u32 index) = 0;
    // TODO: storage buffers
    virtual void BindTexture(TextureBase* texture, ShaderType shader_type,
                             u32 index) = 0;
    // TODO: sampers
    // TODO: images

    // Draw
    virtual void Draw(const Engines::PrimitiveType primitive_type,
                      const u32 start, const u32 count,
                      bool indexed = false) = 0;

    // Getters
    const Info& GetInfo() const { return info; }

    BufferCache& GetBufferCache() { return buffer_cache; }
    TextureCache& GetTextureCache() { return texture_cache; }
    RenderPassCache& GetRenderPassCache() { return render_pass_cache; }
    ShaderCache& GetShaderCache() { return shader_cache; }
    PipelineCache& GetPipelineCache() { return pipeline_cache; }

  protected:
    Info info{};

    // State getters
    // TODO

  private:
    // Caches
    BufferCache buffer_cache;
    TextureCache texture_cache;
    RenderPassCache render_pass_cache;
    ShaderCache shader_cache;
    PipelineCache pipeline_cache;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
