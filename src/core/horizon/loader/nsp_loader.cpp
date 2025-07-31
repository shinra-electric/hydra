#include "core/horizon/loader/nsp_loader.hpp"

namespace hydra::horizon::loader {

NspLoader::NspLoader(const filesystem::PartitionFilesystem& pfs_) : pfs(pfs_) {
    /*
    // TODO: ticket file

    // NCAs
    // HACK: find the largest one
    struct {
        filesystem::FileBase* file;
        usize size;
    } largest_entry{nullptr, 0};
    for (const auto& [filename, entry] : pfs.GetEntries()) {
        auto file = dynamic_cast<filesystem::FileBase*>(entry);
        if (!file)
            continue;

        if (file->GetSize() > largest_entry.size)
            largest_entry = {file, file->GetSize()};
    }

    // TODO: check for encryption
    program_nca_loader = new NcaLoader(largest_entry.file);
    */

    filesystem::EntryBase* main_entry;
    const auto res = pfs.GetEntry("main", main_entry);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to get main entry");
        return;
    }

    auto main_file = dynamic_cast<filesystem::FileBase*>(main_entry);
    if (!main_file) {
        LOG_ERROR(Loader, "Main entry is not a file");
        return;
    }

    nso_loader = new NsoLoader(main_file);
}

} // namespace hydra::horizon::loader
