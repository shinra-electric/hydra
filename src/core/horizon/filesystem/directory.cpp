#include "core/horizon/filesystem/directory.hpp"

#include "core/horizon/filesystem/host_file.hpp"

namespace hydra::horizon::filesystem {

Directory::Directory(const std::string_view host_path) {
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
            if (entry.second && entry.second->IsDirectory())
                return FsResult::DirectoryNotEmpty;
        }
    }

    for (const auto& entry : entries) {
        if (!entry.second)
            continue;

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

FsResult Directory::AddEntry(const std::string_view rel_path, EntryBase* entry,
                             bool add_intermediate) {
    return Find<EntryBase*&>(
        rel_path,
        [entry, add_intermediate](Directory* dir, EntryBase*& out_entry) {
            if (out_entry)
                return FsResult::AlreadyExists;

            out_entry = entry;
            entry->SetParent(dir);
            return FsResult::Success;
        },
        add_intermediate);
}

FsResult Directory::AddEntry(const std::string_view rel_path,
                             const std::string_view host_path,
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

FsResult Directory::DeleteEntry(const std::string_view rel_path,
                                bool recursive) {
    return Find<EntryBase*&>(rel_path,
                             [recursive](Directory* dir, EntryBase*& entry) {
                                 if (!entry)
                                     return FsResult::DoesNotExist;

                                 auto res = entry->Delete(recursive);
                                 delete entry;
                                 entry = nullptr;

                                 return res;
                             });
}

FsResult Directory::GetEntry(const std::string_view rel_path,
                             EntryBase*& out_entry) {
    return Find<EntryBase*>(rel_path,
                            [&out_entry](Directory* dir, EntryBase* entry) {
                                if (!entry) {
                                    out_entry = nullptr;
                                    return FsResult::DoesNotExist;
                                }

                                out_entry = entry;
                                return FsResult::Success;
                            });
}

} // namespace hydra::horizon::filesystem
