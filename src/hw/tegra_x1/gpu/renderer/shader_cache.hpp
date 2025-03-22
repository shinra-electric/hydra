#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class ShaderBase;

struct GuestShaderState {
    Engines::VertexAttribState vertex_attrib_states[VERTEX_ATTRIB_COUNT];
    TextureFormat color_target_formats[COLOR_TARGET_COUNT] = {
        TextureFormat::Invalid};
};

struct GuestShaderDescriptor {
    Engines::ShaderStage stage;
    uptr code_ptr;
    GuestShaderState state;
};

class ShaderCache
    : public CacheBase<ShaderCache, ShaderBase*, GuestShaderDescriptor> {
  public:
    void Destroy() {}

    ShaderBase* Create(const GuestShaderDescriptor& descriptor);
    void Update(ShaderBase* shader) {}
    u64 Hash(const GuestShaderDescriptor& descriptor);

    void DestroyElement(ShaderBase* shader);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
