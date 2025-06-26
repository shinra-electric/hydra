#include "core/horizon/loader/nca_loader.hpp"

#include <stb_image.h>

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/loader/nso_loader.hpp"

#define NINTENDO_LOGO_PATH "logo/NintendoLogo.png"
#define STARTUP_MOVIE_PATH "logo/StartupMovie.gif"

namespace hydra::horizon::loader {

namespace {

enum class NpdmFlags : u8 {
    None = 0,
    Is64BitInstruction = BIT(0),
    AddressSpace32Bit = 0x0 << 1,
    AddressSpace64BitOld = 0x1 << 1,
    AddressSpace32BitNoReserved = 0x2 << 1,
    AddressSpace64Bit = 0x3 << 1,
    OptimizeMemoryAllocation = BIT(4),       // 7.0.0+
    DisableDeviceAddressSpaceMerge = BIT(5), // 11.0.0+
    EnableAliasRegionExtraSize = BIT(6),     // 18.0.0+
    PreventCodeReads = BIT(7),               // 19.0.0+
};
ENABLE_ENUM_BITMASK_OPERATORS(NpdmFlags)

struct NpdmMeta {
    u32 magic;
    u32 signature_key_generation; // 9.0.0+
    u32 _reserved_x8;
    NpdmFlags flags;
    u8 _reserved_xd;
    u8 main_thread_priority;
    u8 main_thread_core_number;
    u32 _reserved_x10;
    u32 system_resource_size; // 3.0.0+
    u32 version;
    u32 main_thread_stack_size;
    char name[0x10];
    u8 product_code[0x10];
    u8 _reserved_x40[0x30];
    u32 aci_offset;
    u32 aci_size;
    u32 acid_offset;
    u32 acid_size;
};

} // namespace

NcaLoader::NcaLoader(const filesystem::ContentArchive& content_archive_)
    : content_archive(content_archive_) {
    filesystem::EntryBase* e;
    const auto res = content_archive.GetEntry("code/main.npdm", e);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to load main.npdm: {}", res);
        return;
    }

    auto file = dynamic_cast<filesystem::FileBase*>(e);
    if (!file) {
        LOG_ERROR(Loader, "main.npdm is not a file");
        return;
    }

    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    const auto meta = reader.Read<NpdmMeta>();

    file->Close(stream);

    ASSERT(meta.magic == make_magic4('M', 'E', 'T', 'A'), Loader,
           "Invalid NPDM meta magic 0x{:08x}", meta.magic);

    // TODO: support 32-bit games
    if (!any(meta.flags & NpdmFlags::Is64BitInstruction)) {
        LOG_WARN(Loader, "32-bit games not supported");
        return;
    }

    LOG_DEBUG(Loader, "Name: {}", meta.name);
    LOG_DEBUG(Loader, "Main thread priority: 0x{:02x}",
              meta.main_thread_priority);
    LOG_DEBUG(Loader, "Main thread core number: {}",
              meta.main_thread_core_number);
    LOG_DEBUG(Loader, "Main thread stack size: 0x{:08x}",
              meta.main_thread_stack_size);
    LOG_DEBUG(Loader, "System resource size: 0x{:08x}",
              meta.system_resource_size);

    name = meta.name;
    main_thread_priority = meta.main_thread_priority;
    main_thread_core_number = meta.main_thread_core_number;
    main_thread_stack_size = meta.main_thread_stack_size;
    system_resource_size = meta.system_resource_size;
}

std::optional<kernel::ProcessParams> NcaLoader::LoadProcess() {
    // Title ID
    KERNEL_INSTANCE.SetTitleId(content_archive.GetTitleID());

    std::optional<kernel::ProcessParams> process_params = std::nullopt;
    for (const auto& [name, entry] : content_archive.GetEntries()) {
        if (name == "code") {
            auto dir = dynamic_cast<filesystem::Directory*>(entry);
            ASSERT(dir, Loader, "Code is not a directory");
            process_params = LoadCode(dir);
        } else if (name.starts_with("data")) {
            const auto res = FILESYSTEM_INSTANCE.AddEntry(
                FS_SD_MOUNT "/rom/romFS", entry, true);
            ASSERT(res == filesystem::FsResult::Success, Loader,
                   "Failed to add romFS entry: {}", res);
        } else if (name == "logo") {
            // Do nothing
        } else {
            LOG_NOT_IMPLEMENTED(Loader, "{}", name);
        }
    }

    CHECK_AND_RETURN_PROCESS_PARAMS(process_params);
}

void NcaLoader::LoadNintendoLogo(uchar4*& out_data, usize& out_width,
                                 usize& out_height) {
    filesystem::EntryBase* logo_entry;
    const auto res = content_archive.GetEntry(NINTENDO_LOGO_PATH, logo_entry);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to get " NINTENDO_LOGO_PATH ": {}", res);
        return;
    }

    auto logo_file = dynamic_cast<filesystem::FileBase*>(logo_entry);
    if (!logo_file) {
        LOG_ERROR(Loader, NINTENDO_LOGO_PATH " is not a file");
        return;
    }

    auto stream = logo_file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    usize raw_data_size = reader.GetSize();
    u8* raw_data = new u8[raw_data_size];
    reader.ReadWhole(raw_data);

    logo_file->Close(stream);

    i32 w, h;
    i32 comp;
    out_data = reinterpret_cast<uchar4*>(stbi_load_from_memory(
        raw_data, raw_data_size, &w, &h, &comp, STBI_rgb_alpha));
    delete[] raw_data;
    if (!out_data) {
        LOG_ERROR(Loader, "Failed to load " NINTENDO_LOGO_PATH);
        return;
    }

    out_width = w;
    out_height = h;
}

void NcaLoader::LoadStartupMovie(
    uchar4*& out_data, std::vector<std::chrono::milliseconds>& out_delays,
    usize& out_width, usize& out_height, u32& out_frame_count) {
    filesystem::EntryBase* logo_entry;
    const auto res = content_archive.GetEntry(STARTUP_MOVIE_PATH, logo_entry);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to get " STARTUP_MOVIE_PATH ": {}", res);
        return;
    }

    auto logo_file = dynamic_cast<filesystem::FileBase*>(logo_entry);
    if (!logo_file) {
        LOG_ERROR(Loader, STARTUP_MOVIE_PATH " is not a file");
        return;
    }

    auto stream = logo_file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    usize raw_data_size = reader.GetSize();
    u8* raw_data = new u8[raw_data_size];
    reader.ReadWhole(raw_data);

    logo_file->Close(stream);

    i32 w, h, f;
    i32 comp;
    i32* delays_ms;
    out_data = reinterpret_cast<uchar4*>(
        stbi_load_gif_from_memory(raw_data, raw_data_size, &delays_ms, &w, &h,
                                  &f, &comp, STBI_rgb_alpha));
    delete[] raw_data;
    if (!out_data) {
        LOG_ERROR(Loader, "Failed to load " STARTUP_MOVIE_PATH);
        return;
    }

    out_width = w;
    out_height = h;
    out_frame_count = f;

    out_delays.reserve(f);
    for (u32 i = 0; i < f; i++)
        out_delays.push_back(std::chrono::milliseconds(delays_ms[i]));
    free(delays_ms);
}

std::optional<kernel::ProcessParams>
NcaLoader::LoadCode(filesystem::Directory* dir) {
    std::optional<kernel::ProcessParams> process_params = std::nullopt;

    // HACK: if rtld is not present, use main as the entry point
    std::string entry_point = "rtld";
    filesystem::EntryBase* e;
    if (dir->GetEntry("rtld", e) == filesystem::FsResult::DoesNotExist)
        entry_point = "main";

    for (const auto& [name, entry] : dir->GetEntries()) {
        auto file = dynamic_cast<filesystem::FileBase*>(entry);
        ASSERT(file, Loader, "Code entry is not a file");
        if (name == "main.npdm") {
            // Do nothing
        } else {
            NsoLoader loader(file, name, name == entry_point);
            auto process_params_ = loader.LoadProcess();
            if (process_params_)
                CHECK_AND_SET_PROCESS_PARAMS(process_params, process_params_);
        }
    }

    ASSERT(process_params, Loader, "Failed to load process");

    process_params->main_thread_priority = main_thread_priority;
    process_params->main_thread_core_number = main_thread_core_number;
    process_params->main_thread_stack_size = main_thread_stack_size;
    process_params->system_resource_size = system_resource_size;

    // TODO: ACI and ACID

    return process_params;
}

} // namespace hydra::horizon::loader
