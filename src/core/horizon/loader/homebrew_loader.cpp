#include "core/horizon/loader/homebrew_loader.hpp"

#include "core/horizon/filesystem/file_view.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/loader/nsp_loader.hpp"

namespace hydra::horizon::loader {

HomebrewLoader::HomebrewLoader(filesystem::FileBase* file_)
    : file{file_}, nro_loader(file) {
    // Asset section
    const auto asset_begin = nro_loader.GetSize();
    TryLoadAssetSection(new filesystem::FileView(
        file, asset_begin, file->GetSize() - asset_begin));
}

void HomebrewLoader::LoadProcess(kernel::Process* process) {
    // Load hbloader
    NspLoader hbloader_loader(
        new filesystem::HostFile(get_bundle_resource_path("hbl.nsp")));
    hbloader_loader.LoadProcess(process);

    // Map the current homebrew file to sdmc:/hbmenu.nro, as that's what
    // hbloader loads by default
    const auto res = FILESYSTEM_INSTANCE.AddEntry("sdmc:/hbmenu.nro", file);
    ASSERT(res == filesystem::FsResult::Success, Loader,
           "Failed to map homebrew file to sdmc:/hbmenu.nro: {}", res);
}

namespace {

struct AssetSection {
    u64 offset;
    u64 size;
};

struct AssetHeader {
    u32 magic;
    u32 format_version;
    AssetSection icon_section;
    AssetSection nacp_section;
    AssetSection romfs_section;
};

} // namespace

void HomebrewLoader::TryLoadAssetSection(filesystem::FileBase* file) {
    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    // Header
    const auto header = reader.Read<AssetHeader>();
    // TODO: is this the correct way to check if the asset section is present?
    if (header.magic != make_magic4('A', 'S', 'E', 'T'))
        return;

    LOG_DEBUG(Loader, "Asset section found");

    // Icon
    if (header.icon_section.size > 0)
        icon_file = new filesystem::FileView(file, header.icon_section.offset,
                                             header.icon_section.size);

    // NACP
    if (header.nacp_section.size > 0) {
        reader.Seek(header.nacp_section.offset);
        // TODO: read
    }

    file->Close(stream);
}

} // namespace hydra::horizon::loader
