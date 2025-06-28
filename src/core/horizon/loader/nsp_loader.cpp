#include "core/horizon/loader/nsp_loader.hpp"

namespace hydra::horizon::loader {

NspLoader::NspLoader(const filesystem::PartitionFilesystem& pfs_) : pfs(pfs_) {
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

    // TODO: needs to be decrypted first
    // main_nca_loader = new NcaLoader(largest_entry.file);
}

} // namespace hydra::horizon::loader
