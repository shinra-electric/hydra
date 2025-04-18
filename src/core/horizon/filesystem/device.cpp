#include "core/horizon/filesystem/device.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

#define VERIFY_PATH(path)                                                      \
    if (path.empty() || path[0] != '/')                                        \
        return FsResult::DoesNotExist;

namespace Hydra::Horizon::Filesystem {

FsResult Device::AddEntry(EntryBase* entry, const std::string& path,
                          bool add_intermediate) {
    VERIFY_PATH(path);
    return root.AddEntry(entry, path.substr(1), add_intermediate);
}

FsResult Device::AddEntry(const std::string& host_path, const std::string& path,
                          bool add_intermediate) {
    VERIFY_PATH(path);
    return root.AddEntry(host_path, path.substr(1), add_intermediate);
}

FsResult Device::GetEntry(const std::string& path, EntryBase*& out_entry) {
    VERIFY_PATH(path);
    return root.GetEntry(path.substr(1), out_entry);
}

} // namespace Hydra::Horizon::Filesystem
