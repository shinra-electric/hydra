#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace hydra::horizon::filesystem {

class Directory : public EntryBase {
  public:
    Directory() = default;
    Directory(const std::string& host_path);
    ~Directory() override;

    bool IsDirectory() const override { return true; }

    FsResult Delete(bool recursive = false) override;

    FsResult AddEntry(const std::string& rel_path, EntryBase* entry,
                      bool add_intermediate = false);
    FsResult AddEntry(const std::string& rel_path, const std::string& host_path,
                      bool add_intermediate = false);
    FsResult DeleteEntry(const std::string& rel_path, bool recursive = false);
    FsResult GetEntry(const std::string& rel_path, EntryBase*& out_entry);

    // Getters
    const std::map<std::string, EntryBase*>& GetEntries() const {
        return entries;
    }

  private:
    std::map<std::string, EntryBase*> entries;

    // Helpers
    FsResult
    Find(const std::string& rel_path,
         const std::function<FsResult(Directory*, EntryBase*&)>& found_callback,
         bool add_intermediate = false) {
        ASSERT(!rel_path.empty(), Filesystem, "Relative path cannot be empty");

        const auto slash_pos = rel_path.find('/');
        if (slash_pos == 0)
            return Find(rel_path.substr(1), found_callback, add_intermediate);

        if (slash_pos == std::string::npos) {
            auto& e = entries[rel_path];
            return found_callback(this, e);
        } else {
            const auto sub_dir_name = rel_path.substr(0, slash_pos);
            const auto next_entry_name = rel_path.substr(slash_pos + 1);

            // Handle special names
            if (sub_dir_name == ".") {
                return Find(next_entry_name, found_callback, add_intermediate);
            } else if (sub_dir_name == "..") {
                if (parent) {
                    return parent->Find(next_entry_name, found_callback,
                                        add_intermediate);
                } else {
                    return FsResult::DoesNotExist;
                }
            }

            // Regular subdirectory
            auto& e = entries[sub_dir_name];
            if (next_entry_name.empty()) {
                return found_callback(this, e);
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

                return sub_dir->Find(next_entry_name, found_callback,
                                     add_intermediate);
            }
        }
    }
};

} // namespace hydra::horizon::filesystem
