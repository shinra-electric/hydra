#include "core/hw/tegra_x1/gpu/renderer/texture_cache.hpp"

#include "common/time.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

TextureCache::~TextureCache() {
    // TODO: uncomment
    /*
    for (auto& [key, value] : texture_mem_map) {
        for (auto& [key, value] : value.cache) {
            delete value.base;
            for (auto& [key, value] : value.view_cache)
                delete value;
        }
    }
    */
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

void TextureCache::NotifyGuestModifiedData(const range<uptr> mem_range) {
    // TODO: mark all overlapping memories as modified
    auto it = texture_mem_map.find(mem_range.begin);
    if (it == texture_mem_map.end())
        return;

    it->second.info.MarkModified();
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
            // Presenting, but never written to
            sync = true;
        } else if (usage == TextureUsage::Read) {
            // Read, but never written to

            // Check if the data hash needs to be checked
            auto& data_hash = info.data_hash;
            if (data_hash.ShouldCheck()) {
                u64 data_hash = GetTextureDataHash(tex.base);
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

    if (sync)
        DecodeTexture(tex, info, update_data_hash);

    if (usage == TextureUsage::Read)
        info.MarkRead();
    else if (usage == TextureUsage::Write)
        info.MarkWritten();
}

u64 TextureCache::GetTextureHash(const TextureDescriptor& descriptor) {
    u64 hash = 0;
    hash += descriptor.ptr;
    hash = std::rotl(hash, 7);
    hash += descriptor.width;
    hash = std::rotl(hash, 11);
    hash += descriptor.height;
    hash = std::rotl(hash, 11);

    // View compatbility hash
    // TODO: get format info from the renderer instead
    hash += is_texture_format_compressed(descriptor.format);
    hash = std::rotl(hash, 1);
    hash += is_texture_format_depth_or_stencil(descriptor.format);
    hash = std::rotl(hash, 1);

    return hash;
}

u64 TextureCache::GetTextureDataHash(const TextureBase* texture) {
    constexpr u32 SAMPLE_COUNT = 37;

    const auto& descriptor = texture->GetDescriptor();
    u64 mem_range = descriptor.stride * descriptor.height;
    u64 mem_step = std::max(mem_range / SAMPLE_COUNT, 1ull);

    u64 hash = 0;
    for (u64 offset = 0; offset < mem_range; offset += mem_step) {
        hash += *reinterpret_cast<u64*>(descriptor.ptr + offset);
        hash = std::rotl(hash, 7);
    }

    return hash;
}

void TextureCache::DecodeTexture(Tex& tex, TextureMemInfo& info,
                                 bool update_data_hash) {
    // Align the height to 16 bytes (TODO: why 16?)
    auto tmp_buffer = RENDERER_INSTANCE.AllocateTemporaryBuffer(
        tex.base->GetDescriptor().stride *
        align(tex.base->GetDescriptor().height, 16ull));
    texture_decoder.Decode(tex.base->GetDescriptor(),
                           (u8*)tmp_buffer->GetDescriptor().ptr);
    tex.base->CopyFrom(tmp_buffer);
    RENDERER_INSTANCE.FreeTemporaryBuffer(tmp_buffer);

    // Update metadata
    tex.MarkCpuSynced();
    if (update_data_hash)
        info.data_hash.Update(GetTextureDataHash(tex.base));
    tex.data_hash = info.data_hash.hash;
}

} // namespace hydra::hw::tegra_x1::gpu::renderer
