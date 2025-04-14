#include "core/horizon/filesystem/directory.hpp"

#include "core/horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Filesystem {

Directory::Directory(Directory* parent_, const std::string& host_path)
    : parent{parent_} {
    ASSERT(std::filesystem::is_directory(host_path), HorizonFilesystem,
           "\"{}\" is not a directory", host_path);

    for (const auto& entry : std::filesystem::directory_iterator(host_path)) {
        const auto& entry_path = entry.path().string();
        const auto entry_name =
            entry_path.substr(entry_path.find_last_of("/") + 1);
        AddEntry(entry_path, entry_name);
    }
}

Directory::~Directory() {
    for (const auto [name, entry] : entries)
        delete entry;
}

void Directory::AddEntry(EntryBase* entry, const std::string& rel_path) {
    ASSERT(rel_path.size() != 0, HorizonFilesystem,
           "Relative path cannot be empty");

    const auto slash_pos = rel_path.find('/');
    if (slash_pos == std::string::npos) {
        entries[rel_path] = entry;
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto sub_dir = new Directory(this);
        entries[sub_dir_name] = sub_dir;
        sub_dir->AddEntry(entry, rel_path.substr(slash_pos + 1));
    }
}

void Directory::AddEntry(const std::string& host_path,
                         const std::string& rel_path) {
    ASSERT(std::filesystem::exists(host_path), HorizonFilesystem,
           "Host path \"{}\" does not exist", host_path);

    EntryBase* entry{nullptr};
    if (std::filesystem::is_directory(host_path)) {
        entry = new Directory(this, host_path);
    } else if (std::filesystem::is_regular_file(host_path)) {
        entry = new File(host_path);
    } else {
        LOG_ERROR(HorizonFilesystem, "Invalid host path \"{}\"", host_path);
    }

    AddEntry(entry, rel_path);
}

EntryBase* Directory::GetEntry(const std::string& rel_path) {
    if (rel_path.empty())
        return this;

    const auto slash_pos = rel_path.find('/');

    // If there is no slash, or the slash is at the end of the string,
    // return the entry directly
    if (slash_pos == std::string::npos) {
        return GetEntryImpl(rel_path);
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto next_entry_name = rel_path.substr(slash_pos + 1);

        // Handle special names
        if (sub_dir_name == ".") {
            return GetEntry(next_entry_name);
        } else if (sub_dir_name == "..") {
            if (parent)
                return parent->GetEntry(next_entry_name);
            else
                return nullptr;
        }

        // Regular subdirectory
        const auto sub_dir =
            dynamic_cast<Directory*>(GetEntryImpl(sub_dir_name));
        if (!sub_dir)
            return nullptr;

        return sub_dir->GetEntry(next_entry_name);
    }
}

EntryBase* Directory::GetEntryImpl(const std::string& name) {
    auto it = entries.find(name);
    if (it == entries.end())
        return nullptr;

    return it->second;
}

} // namespace Hydra::Horizon::Filesystem
