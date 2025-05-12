#include "core/horizon/filesystem/device.hpp"

#include "core/horizon/filesystem/directory.hpp"

#define VERIFY_PATH(path)                                                      \
    if (path.empty() || path[0] != '/')                                        \
        return FsResult::DoesNotExist;

namespace hydra::horizon::filesystem {

Device::~Device() { delete root; }

FsResult Device::AddEntry(const std::string& path, EntryBase* entry,
                          bool add_intermediate) {
    VERIFY_PATH(path);
    return root->AddEntry(path.substr(1), entry, add_intermediate);
}

FsResult Device::AddEntry(const std::string& path, const std::string& host_path,
                          bool add_intermediate) {
    VERIFY_PATH(path);
    return root->AddEntry(path.substr(1), host_path, add_intermediate);
}

FsResult Device::GetEntry(const std::string& path, EntryBase*& out_entry) {
    VERIFY_PATH(path);
    return root->GetEntry(path.substr(1), out_entry);
}

} // namespace hydra::horizon::filesystem
