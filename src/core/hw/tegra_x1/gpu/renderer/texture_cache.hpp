#pragma once

#include "core/hw/tegra_x1/gpu/renderer/texture_decoder.hpp"

namespace hydra::hw::tegra_x1::cpu {
class MMUBase;
}

namespace hydra::hw::tegra_x1::gpu::renderer {

class TextureBase;

struct Tex {
    TextureBase* base{nullptr};
    small_cache<u32, TextureBase*> view_cache;
};

struct TextureMem {
    small_cache<u64, Tex> cache;
};

class TextureCache {
  public:
    ~TextureCache();

    TextureBase* GetTextureView(const TextureDescriptor& descriptor);

  private:
    TextureDecoder texture_decoder;

    // TODO: use a more memory lookup friendly data structure
    std::map<uptr, TextureMem> texture_mem_map;

    // Buffers
    std::vector<u8> scratch_buffer;

    TextureBase* Create(const TextureDescriptor& descriptor);
    void Update(TextureBase* texture);

    // Helpers
    u64 GetTextureHash(const TextureDescriptor& descriptor);
    void DecodeTexture(TextureBase* texture);
    // TODO: encode texture
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
