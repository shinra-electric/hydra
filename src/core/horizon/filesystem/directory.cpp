#include "core/horizon/filesystem/directory.hpp"

#include "core/horizon/filesystem/host_file.hpp"

namespace hydra::horizon::filesystem {

Directory::Directory(const std::string& host_path) {
    ASSERT(std::filesystem::is_directory(host_path), Filesystem,
           "\"{}\" is not a directory", host_path);

    for (const auto& entry : std::filesystem::directory_iterator(host_path)) {
        const auto& entry_path = entry.path().string();
        const auto entry_name =
            entry_path.substr(entry_path.find_last_of("/") + 1);

        // Ignore certain entries
        if (entry_name == ".DS_Store")
            continue;

        AddEntry(entry_name, entry_path);
    }
}

Directory::~Directory() {
    for (const auto [name, entry] : entries)
        delete entry;
}

FsResult Directory::Delete(bool recursive) {
    if (!recursive) {
        for (const auto& entry : entries) {
            if (entry.second->IsDirectory())
                return FsResult::DirectoryNotEmpty;
        }
    }

    for (const auto& entry : entries) {
        if (entry.second->IsDirectory()) {
            auto dir = dynamic_cast<Directory*>(entry.second);
            ASSERT_DEBUG(dir, Filesystem, "This should not happen");
            dir->Delete(true);
            delete dir;
        } else {
            auto file = dynamic_cast<FileBase*>(entry.second);
            ASSERT_DEBUG(file, Filesystem, "This should not happen");
            file->Delete();
            delete file;
        }
    }

    entries.clear();
    return FsResult::Success;
}

FsResult Directory::AddEntry(const std::string& rel_path, EntryBase* entry,
                             bool add_intermediate) {
    ASSERT(rel_path.size() != 0, Filesystem, "Relative path cannot be empty");

    const auto slash_pos = rel_path.find('/');
    if (slash_pos == 0)
        return AddEntry(rel_path.substr(1), entry, add_intermediate);

    if (slash_pos == std::string::npos) {
        auto& e = entries[rel_path];
        if (e)
            return FsResult::AlreadyExists;

        e = entry;
        entry->SetParent(this);
    } else {
        const auto sub_dir_name = rel_path.substr(0, slash_pos);
        const auto next_entry_name = rel_path.substr(slash_pos + 1);

        // Handle special names
        if (sub_dir_name == ".") {
            return AddEntry(next_entry_name, entry, add_intermediate);
        } else if (sub_dir_name == "..") {
            if (parent) {
                return parent->AddEntry(next_entry_name, entry,
                                        add_intermediate);
            } else {
                return FsResult::DoesNotExist;
            }
        }

        // Regular subdirectory
        auto& e = entries[sub_dir_name];
        if (next_entry_name.empty()) {
            if (e)
                return FsResult::AlreadyExists;

            e = entry;
            entry->SetParent(this);
        } else {
            if (!e) {
                if (add_intermediate) {
                    e = new Directory();
                    e->SetParent(this);
                } else {
                    return FsResult::IntermediateDirectoryDoesNotExist;
                }
            }

            auto sub_dir = dynamic_cast<Directory*>(e);
            if (!sub_dir)
                return FsResult::NotADirectory;

            return sub_dir->AddEntry(next_entry_name, entry, add_intermediate);
        }
    }

    return FsResult::Success;
}

FsResult Directory::AddEntry(const std::string& rel_path,
                             const std::string& host_path,
                             bool add_intermediate) {
    ASSERT(std::filesystem::exists(host_path), Filesystem,
           "Host path \"{}\" does not exist", host_path);

    EntryBase* entry{nullptr};
    if (std::filesystem::is_directory(host_path)) {
        entry = new Directory(host_path);
    } else if (std::filesystem::is_regular_file(host_path)) {
        entry = new HostFile(host_path);
    } else {
        LOG_ERROR(Filesystem, "Invalid host path \"{}\"", host_path);
    }

    return AddEntry(rel_path, entry, add_intermediate);
}

FsResult Directory::GetEntry(const std::string& rel_path,
                             EntryBase*& out_entry) {
    if (rel_path.empty()) {
        out_entry = this;
        return FsResult::Success;
    }

    const auto slash_pos = rel_path.find('/');
    if (slash_pos == 0)
        return GetEntry(rel_path.substr(1), out_entry);

    // If there is no slash, or the slash is at the end of the string,
    // return the entry directly
    if (slash_pos == std::string::npos) {
        out_entry = GetEntryImpl(rel_path);
        if (!out_entry) {
            out_entry = nullptr;
            return FsResult::DoesNotExist;
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
                return FsResult::DoesNotExist;
            }
        }

        // Regular subdirectory
        EntryBase* sub_dir = GetEntryImpl(sub_dir_name);
        if (!sub_dir) {
            out_entry = nullptr;
            return FsResult::DoesNotExist;
        }

        auto sub_dir_impl = dynamic_cast<Directory*>(sub_dir);
        if (!sub_dir_impl) {
            out_entry = nullptr;
            return FsResult::NotADirectory;
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

} // namespace hydra::horizon::filesystem
