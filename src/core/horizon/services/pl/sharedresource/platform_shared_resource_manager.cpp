#include "core/horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"

#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::services::pl::shared_resource {

namespace {

constexpr usize SHARED_MEMORY_SIZE = 0x01100000;

#define SHARED_FONT_ENTRY(name, filename) [(u32)SharedFontType::name] = filename

constexpr std::string_view shared_font_filenames[] = {
    SHARED_FONT_ENTRY(JapanUsEurope, "FontStandard"),
    SHARED_FONT_ENTRY(ChineseSimplified, "FontChineseSimple"),
    SHARED_FONT_ENTRY(ExtendedChineseSimplified, "FontChineseSimple"),
    SHARED_FONT_ENTRY(ChineseTraditional, "FontChineseTraditional"),
    SHARED_FONT_ENTRY(Korean, "FontKorean"),
    SHARED_FONT_ENTRY(NintendoExtended, "FontNintendoExtension"),
};

#undef SHARED_FONT_ENTRY

filesystem::FileBase* GetSharedFontFile(SharedFontType font_type) {
    filesystem::FileBase* file;
    const auto res = FILESYSTEM_INSTANCE.GetFile(
        fmt::format(FS_FIRMWARE_PATH "/{}",
                    shared_font_filenames[(u32)font_type]),
        file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Services, "Failed to get shared font {} file", font_type);
        return nullptr;
    }

    return file;
}

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IPlatformSharedResourceManager, 0, RequestLoad, 1,
                             GetLoadState, 2, GetSize, 3,
                             GetSharedMemoryAddressOffset, 4,
                             GetSharedMemoryNativeHandle, 5,
                             GetSharedFontInOrderOfPriority)

IPlatformSharedResourceManager::IPlatformSharedResourceManager()
    : shared_memory_handle(new kernel::SharedMemory(SHARED_MEMORY_SIZE)) {}

result_t IPlatformSharedResourceManager::RequestLoad(SharedFontType font_type) {
    auto& state = states[(u32)font_type];
    if (state.load_state == LoadState::Loaded) {
        LOG_WARN(Services, "Shared font {} is already loaded", font_type);
        return RESULT_SUCCESS;
    }

    auto file = GetSharedFontFile(font_type);
    if (!file)
        return MAKE_RESULT(Fs, 1); // TODO

    // Load
    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    reader.ReadWhole((u8*)shared_memory_handle.handle->GetPtr() +
                     shared_memory_offset);

    file->Close(stream);

    // Set state
    state.load_state = LoadState::Loaded;
    state.shared_memory_offset = shared_memory_offset;
    state.size = file->GetSize();
    shared_memory_offset += file->GetSize();

    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetLoadState(SharedFontType font_type,
                                                      LoadState* out_state) {
    *out_state = states[(u32)font_type].load_state;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSize(SharedFontType font_type,
                                                 u32* out_size) {
    *out_size = states[(u32)font_type].size;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedMemoryAddressOffset(
    SharedFontType font_type, u32* out_address_offset) {
    *out_address_offset = states[(u32)font_type].shared_memory_offset;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedMemoryNativeHandle(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = shared_memory_handle.id;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedFontInOrderOfPriority(
    LanguageCode language_code, u8* out_loaded, u32* out_count,
    OutBuffer<BufferAttr::MapAlias> out_types_buffer,
    OutBuffer<BufferAttr::MapAlias> out_offsets_buffer,
    OutBuffer<BufferAttr::MapAlias> out_sizes_buffer) {
    *out_loaded = 0;
    *out_count = 0;

    // HACK: return early, nx-hbmenu probably doesn't like the font data
    return RESULT_SUCCESS;

    for (SharedFontType type = (SharedFontType)0;
         type < SharedFontType::ChineseSimplified; type++) {
        const auto& state = states[(u32)type];
        if (state.load_state != LoadState::Loaded)
            continue;

        out_types_buffer.writer->Write(type);
        out_offsets_buffer.writer->Write<u32>(state.shared_memory_offset);
        out_sizes_buffer.writer->Write<u32>(state.size);

        (*out_loaded)++;
        (*out_count)++;
    }

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::pl::shared_resource
