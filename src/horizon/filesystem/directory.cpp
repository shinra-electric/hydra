#include "horizon/filesystem/directory.hpp"

#define VERIFY_PATH(path)                                                      \
    ASSERT(rel_path.size() != 0, HorizonFilesystem,                            \
           "Relative path cannot be empty");

namespace Hydra::Horizon::Filesystem {

Directory::Directory(const std::string& host_path) {
    // TODO: implement
    LOG_FUNC_STUBBED(HorizonFilesystem);
}

Directory::~Directory() {
    for (const auto [name, entry] : entries)
        delete entry;
}

void Directory::AddEntry(EntryBase* entry, const std::string& rel_path) {
    VERIFY_PATH(rel_path);
    const auto slash_pos = rel_path.find('/');
    if (slash_pos == std::string::npos) {
        entries[rel_path] = entry;
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto sub_dir = new Directory();
        entries[sub_dir_name] = sub_dir;
        sub_dir->AddEntry(entry, rel_path.substr(slash_pos + 1));
    }
}

EntryBase* Directory::GetEntry(const std::string& rel_path) {
    VERIFY_PATH(rel_path);
    const auto slash_pos = rel_path.find('/');
    if (slash_pos == std::string::npos) {
        return GetEntryImpl(rel_path);
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto sub_dir =
            dynamic_cast<Directory*>(GetEntryImpl(sub_dir_name));
        if (!sub_dir)
            return nullptr;

        return sub_dir->GetEntry(rel_path.substr(slash_pos + 1));
    }
}

EntryBase* Directory::GetEntryImpl(const std::string& name) {
    auto it = entries.find(name);
    if (it == entries.end())
        return nullptr;

    return it->second;
}

} // namespace Hydra::Horizon::Filesystem
