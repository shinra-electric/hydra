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
    SmallCache<u32, TextureBase*> view_cache;
    TextureCacheTimePoint cpu_sync_timestamp{};
    u32 data_hash{0};

    void MarkCpuSynced() { cpu_sync_timestamp = TextureCacheClock::now(); }
};

struct DataHash {
    static constexpr f32 MIN_SUCCESS_RATE = 0.2f;
    static constexpr f32 MIX_AMOUNT = 0.1f;

    u32 hash{0};
    TextureCacheTimePoint updated_timestamp{};
    f32 check_success_rate{MIN_SUCCESS_RATE};

    bool ShouldCheck() const {
        // The farther away the success rate is from MIN_SUCCESS_RATE, the
        // longer the check interval
        f32 amount;
        if (check_success_rate >= MIN_SUCCESS_RATE)
            amount = (check_success_rate - MIN_SUCCESS_RATE) /
                     (1.0f - MIN_SUCCESS_RATE);
        else
            amount = (MIN_SUCCESS_RATE - check_success_rate) / MIN_SUCCESS_RATE;
        f32 amount_squared = amount * amount;
        std::chrono::milliseconds check_interval =
            std::chrono::milliseconds(static_cast<i32>(amount_squared * 10000));

        return TextureCacheClock::now() > updated_timestamp + check_interval;
    }

    void Update(u32 hash_) {
        hash = hash_;
        updated_timestamp = TextureCacheClock::now();
        check_success_rate =
            check_success_rate * (1.0f - MIX_AMOUNT) + MIX_AMOUNT;
    }

    void NotifyNotChanged() { check_success_rate *= 1.0f - MIX_AMOUNT; }
};

struct TextureMemInfo {
    TextureCacheTimePoint modified_timestamp{};
    TextureCacheTimePoint read_timestamp{};
    TextureCacheTimePoint written_timestamp{};
    DataHash data_hash{};

    void MarkModified() { modified_timestamp = TextureCacheClock::now(); }
    void MarkRead() { read_timestamp = TextureCacheClock::now(); }
    void MarkWritten() { written_timestamp = TextureCacheClock::now(); }
};

struct TextureMem {
    TextureMemInfo info;
    SmallCache<u32, Tex> cache;
};

// TODO: track GPU modifications as well?
class TextureCache {
  public:
    ~TextureCache();

    TextureBase* GetTextureView(const TextureDescriptor& descriptor,
                                TextureUsage usage);

    void InvalidateMemory(Range<uptr> range);

  private:
    TextureDecoder texture_decoder;

    // TODO: use a more memory lookup friendly data structure
    std::map<uptr, TextureMem> texture_mem_map;

    void Create(const TextureDescriptor& descriptor, Tex& tex,
                TextureMemInfo& info);
    void Update(Tex& tex, TextureMemInfo& info, TextureUsage usage);

    // Helpers
    u32 GetTextureHash(const TextureDescriptor& descriptor);
    u32 GetDataHash(const TextureBase* texture);
    void DecodeTexture(Tex& tex, TextureMemInfo& info,
                       bool update_data_hash = true);
    // TODO: encode texture
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
