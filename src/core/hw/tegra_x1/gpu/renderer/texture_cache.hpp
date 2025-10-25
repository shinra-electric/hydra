#pragma once

#include "core/hw/tegra_x1/gpu/renderer/texture_decoder.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu::renderer {

class TextureBase;

typedef std::chrono::steady_clock TextureCacheClock;
typedef TextureCacheClock::time_point TextureCacheTimePoint;

struct Tex {
    TextureBase* base{nullptr};
    small_cache<u32, TextureBase*> view_cache;
    TextureCacheTimePoint cpu_sync_timestamp{};
    u64 data_hash{0};

    void MarkCpuSynced() { cpu_sync_timestamp = TextureCacheClock::now(); }
};

enum class DataHashCheckStrategy {
    Always,
    OnceInAWhileOrIgnore,
    OnceInAWhileOrForceSync,
};

struct TextureMemInfo {
    TextureCacheTimePoint modified_timestamp{};
    TextureCacheTimePoint read_timestamp{};
    TextureCacheTimePoint written_timestamp{};

    // Data hash
    DataHashCheckStrategy data_hash_check_strategy{
        DataHashCheckStrategy::Always};
    u64 data_hash{0};
    TextureCacheTimePoint data_hash_updated_timestamp{};

    void MarkModified() { modified_timestamp = TextureCacheClock::now(); }
    void MarkRead() { read_timestamp = TextureCacheClock::now(); }
    void MarkWritten() { written_timestamp = TextureCacheClock::now(); }
    void UpdateDataHash(u64 data_hash_) {
        data_hash = data_hash_;
        data_hash_updated_timestamp = TextureCacheClock::now();
    }
};

struct TextureMem {
    TextureMemInfo info;
    small_cache<u64, Tex> cache;
};

// TODO: track GPU modifications as well?
class TextureCache {
  public:
    ~TextureCache();

    TextureBase* GetTextureView(const TextureDescriptor& descriptor,
                                TextureUsage usage);

    void NotifyGuestModifiedData(const range<uptr> mem_range);

  private:
    TextureDecoder texture_decoder;

    // TODO: use a more memory lookup friendly data structure
    std::map<uptr, TextureMem> texture_mem_map;

    void Create(const TextureDescriptor& descriptor, Tex& tex,
                TextureMemInfo& info);
    void Update(Tex& tex, TextureMemInfo& info, TextureUsage usage);

    // Helpers
    u64 GetTextureHash(const TextureDescriptor& descriptor);
    u64 GetTextureDataHash(const TextureBase* texture);
    void DecodeTexture(Tex& tex, TextureMemInfo& info, bool update_data_hash);
    // TODO: encode texture
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
