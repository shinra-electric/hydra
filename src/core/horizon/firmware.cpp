#include "core/horizon/firmware.hpp"

#include "core/horizon/filesystem/content_archive.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"

namespace hydra::horizon {

void try_install_firmware_to_filesystem(filesystem::Filesystem& fs) {
    const auto& firmware_path = CONFIG_INSTANCE.GetFirmwarePath().Get();
    if (!std::filesystem::exists(firmware_path)) {
        LOG_WARN(Horizon, "Firmware path does not exist");
        return;
    }

    std::map<std::string, std::string> firmware_titles_map = {
        {"010000000000080a/data", "AvatarImage"},
        {"010000000000080e/data", "TimeZoneBinary"},
        {"0100000000000810/data", "FontNintendoExtension"},
        {"0100000000000811/data", "FontStandard"},
        {"0100000000000812/data", "FontKorean"},
        {"0100000000000813/data", "FontChineseTraditional"},
        {"0100000000000814/data", "FontChineseSimple"},
    };

    // Iterate over the directory
    for (const auto& entry :
         std::filesystem::directory_iterator(firmware_path)) {
        auto file = new horizon::filesystem::HostFile(entry.path().string());
        horizon::filesystem::ContentArchive content_archive(file);

        auto res = fs.AddEntry(fmt::format(FS_FIRMWARE_PATH "/{:016x}/{}",
                                           content_archive.GetTitleID(),
                                           content_archive.GetContentType()),
                               file, true);
        ASSERT(res == horizon::filesystem::FsResult::Success, Horizon,
               "Failed to add firmware entry {:016x}: {}",
               content_archive.GetTitleID(), res);
    }

    for (const auto& [path, filename] : firmware_titles_map) {
        filesystem::FileBase* file;
        auto res = fs.GetFile(fmt::format(FS_FIRMWARE_PATH "/{}", path), file);
        ASSERT(res == horizon::filesystem::FsResult::Success, Horizon,
               "Failed to get firmware entry {}: {}", path, res);

        res = fs.AddEntry(fmt::format(FS_FIRMWARE_PATH "/{}", filename), file,
                          true);
        ASSERT(res == horizon::filesystem::FsResult::Success, Horizon,
               "Failed to add firmware entry alias \"{}\": {}", filename, res);
    }
}

} // namespace hydra::horizon
