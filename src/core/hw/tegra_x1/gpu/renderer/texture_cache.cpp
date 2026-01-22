#include "core/hw/tegra_x1/gpu/renderer/texture_cache.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

TextureCache::~TextureCache() {
    for (auto& [key, value] : texture_mem_map) {
        for (const auto [key, value] : value.cache) {
            delete value.base;
            for (const auto [key, value] : value.view_cache)
                delete value;
        }
    }
}

TextureBase* TextureCache::GetTextureView(const TextureDescriptor& descriptor,
                                          TextureUsage usage) {
    auto& texture_mem = texture_mem_map[descriptor.ptr];
    auto& tex = texture_mem.cache.Find(GetTextureHash(descriptor));
    if (!tex.base)
        Create(descriptor, tex, texture_mem.info);

    Update(tex, texture_mem.info, usage);

    // If the formats match and swizzle is the default swizzle, return base
    if (descriptor.format == tex.base->GetDescriptor().format &&
        descriptor.swizzle_channels ==
            tex.base->GetDescriptor().swizzle_channels) {
        return tex.base;
    }

    // Otherwise, get a texture view
    auto view_desc = TextureViewDescriptor{
        .format = descriptor.format,
        .swizzle_channels = descriptor.swizzle_channels,
    };
    auto& view = tex.view_cache.Find(view_desc.GetHash());
    if (view)
        return view;

    view = tex.base->CreateView(view_desc);

    return view;
}

void TextureCache::InvalidateMemory(Range<uptr> range) {
    auto it = texture_mem_map.upper_bound(range.GetBegin());
    if (it != texture_mem_map.begin())
        it--;

    for (; it != texture_mem_map.end() && it->first < range.GetEnd(); it++) {
        auto& mem = it->second;

        // We assume that textures that have been written to by the GPU are
        // never modified by the CPU
        if (mem.info.written_timestamp != TextureCacheTimePoint{})
            continue;

        // Check if its in the range
        const auto& descriptor =
            (*mem.cache.begin()).second.base->GetDescriptor();
        const auto size = descriptor.height * descriptor.stride;
        if (it->first + size > range.GetBegin())
            mem.info.MarkModified();
    }
}

void TextureCache::Create(const TextureDescriptor& descriptor, Tex& tex,
                          TextureMemInfo& info) {
    auto desc = descriptor;
    desc.swizzle_channels =
        get_texture_format_default_swizzle_channels(desc.format);
    tex.base = RENDERER_INSTANCE.CreateTexture(desc);
    DecodeTexture(tex, info);
}

void TextureCache::Update(Tex& tex, TextureMemInfo& info, TextureUsage usage) {
    bool sync = false;
    bool update_data_hash = true;
    if (tex.cpu_sync_timestamp < info.modified_timestamp) {
        // If modified by the guest
        sync = true;
    } else if (tex.data_hash != info.data_hash.hash) {
        // Data changed, but this texture hasn't been updated yet
        sync = true;
    } else if (info.written_timestamp == TextureCacheTimePoint{}) {
        // Never written to
        if (usage == TextureUsage::Present) {
            // Presented, but never written to
            sync = true;
        } else if (usage == TextureUsage::Read) {
            // Read, but never written to

            if (false) { // TODO: if low GPU accuracy
                // Check if the data hash needs to be checked
                auto& data_hash = info.data_hash;
                if (data_hash.ShouldCheck()) {
                    u32 data_hash = GetDataHash(tex.base);
                    if (data_hash != info.data_hash.hash) {
                        sync = true;
                        update_data_hash = false;
                        info.data_hash.Update(data_hash);
                    } else {
                        info.data_hash.NotifyNotChanged();
                    }
                } else if (data_hash.check_success_rate >=
                           DataHash::MIN_SUCCESS_RATE) {
                    // If there is a high chance that the data has changed
                    sync = true;
                    update_data_hash = false;
                }
            }
        }
    }

    if (sync)
        DecodeTexture(tex, info, update_data_hash);

    if (usage == TextureUsage::Read)
        info.MarkRead();
    else if (usage == TextureUsage::Write)
        info.MarkWritten();
}

u32 TextureCache::GetTextureHash(const TextureDescriptor& descriptor) {
    HashCode hash;
    hash.Add(descriptor.width);
    hash.Add(descriptor.height);

    // View compatbility hash
    // TODO: get format info from the renderer instead
    hash.Add(is_texture_format_compressed(descriptor.format));
    hash.Add(is_texture_format_depth_or_stencil(descriptor.format));
    hash.Add(get_texture_format_stride(descriptor.format, 16));

    return hash.ToHashCode();
}

u32 TextureCache::GetDataHash(const TextureBase* texture) {
    constexpr u32 SAMPLE_COUNT = 37;

    const auto& descriptor = texture->GetDescriptor();
    u64 mem_range = descriptor.stride * descriptor.height;
    u64 mem_step = std::max(mem_range / SAMPLE_COUNT, 1ull);

    HashCode hash;
    for (u64 offset = 0; offset < mem_range; offset += mem_step)
        hash.Add(*reinterpret_cast<u64*>(descriptor.ptr + offset));

    return hash.ToHashCode();
}

void TextureCache::DecodeTexture(Tex& tex, TextureMemInfo& info,
                                 bool update_data_hash) {
    const auto& descriptor = tex.base->GetDescriptor();

    // Align the height to 16 bytes (TODO: why 16?)
    auto tmp_buffer = RENDERER_INSTANCE.AllocateTemporaryBuffer(
        descriptor.stride * align(descriptor.height, 16u));
    texture_decoder.Decode(descriptor, (u8*)tmp_buffer->GetPtr());
    tex.base->CopyFrom(tmp_buffer);
    RENDERER_INSTANCE.FreeTemporaryBuffer(tmp_buffer);

    // Update metadata
    tex.MarkCpuSynced();
    if (false) { // TODO: if low GPU accuracy
        if (update_data_hash)
            info.data_hash.Update(GetDataHash(tex.base));
        tex.data_hash = info.data_hash.hash;
    }
}

} // namespace hydra::hw::tegra_x1::gpu::renderer
