#include "core/horizon/filesystem/directory.hpp"

#include "core/horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Filesystem {

Directory::Directory(const std::string& host_path) {
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

FsResult Directory::AddEntry(EntryBase* entry, const std::string& rel_path) {
    ASSERT(rel_path.size() != 0, HorizonFilesystem,
           "Relative path cannot be empty");

    const auto slash_pos = rel_path.find('/');
    if (slash_pos == std::string::npos) {
        auto& e = entries[rel_path];
        if (e)
            return FsResult::AlreadyExists;

        e = entry;
        entry->SetParent(this);
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto next_entry_name = rel_path.substr(slash_pos + 1);

        auto& e = entries[sub_dir_name];
        if (next_entry_name.empty()) {
            if (e)
                return FsResult::AlreadyExists;

            e = entry;
            entry->SetParent(this);
        } else {
            if (e) {
                auto sub_dir = dynamic_cast<Directory*>(e);
                if (!sub_dir)
                    return FsResult::PathIsNotDirectory;

                return sub_dir->AddEntry(entry, next_entry_name);
            } else {
                // TODO: add an option to create intermediate directories
                return FsResult::PathDoesNotExist;
            }
        }
    }

    return FsResult::Success;
}

FsResult Directory::AddEntry(const std::string& host_path,
                             const std::string& rel_path) {
    ASSERT(std::filesystem::exists(host_path), HorizonFilesystem,
           "Host path \"{}\" does not exist", host_path);

    EntryBase* entry{nullptr};
    if (std::filesystem::is_directory(host_path)) {
        entry = new Directory(host_path);
    } else if (std::filesystem::is_regular_file(host_path)) {
        entry = new File(host_path);
    } else {
        LOG_ERROR(HorizonFilesystem, "Invalid host path \"{}\"", host_path);
    }

    return AddEntry(entry, rel_path);
}

FsResult Directory::GetEntry(const std::string& rel_path,
                             EntryBase*& out_entry) {
    if (rel_path.empty()) {
        out_entry = this;
        return FsResult::Success;
    }

    const auto slash_pos = rel_path.find('/');

    // If there is no slash, or the slash is at the end of the string,
    // return the entry directly
    if (slash_pos == std::string::npos) {
        out_entry = GetEntryImpl(rel_path);
        if (!out_entry) {
            out_entry = nullptr;
            return FsResult::PathDoesNotExist;
        }
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto next_entry_name = rel_path.substr(slash_pos + 1);

        // Handle special names
        if (sub_dir_name == ".") {
            return GetEntry(next_entry_name, out_entry);
        } else if (sub_dir_name == "..") {
            if (parent) {
                return parent->GetEntry(next_entry_name, out_entry);
            } else {
                out_entry = nullptr;
                return FsResult::PathDoesNotExist;
            }
        }

        // Regular subdirectory
        EntryBase* sub_dir = GetEntryImpl(sub_dir_name);
        if (!sub_dir) {
            out_entry = nullptr;
            return FsResult::PathDoesNotExist;
        }

        auto sub_dir_impl = dynamic_cast<Directory*>(sub_dir);
        if (!sub_dir_impl) {
            out_entry = nullptr;
            return FsResult::PathIsNotDirectory;
        }

        return sub_dir_impl->GetEntry(next_entry_name, out_entry);
    }

    return FsResult::Success;
}

EntryBase* Directory::GetEntryImpl(const std::string& name) {
    auto it = entries.find(name);
    if (it == entries.end())
        return nullptr;

    return it->second;
}

} // namespace Hydra::Horizon::Filesystem
