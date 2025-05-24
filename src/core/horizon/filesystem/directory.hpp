#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace hydra::horizon::filesystem {

class Directory : public EntryBase {
  public:
    Directory() = default;
    Directory(const std::string_view host_path);
    ~Directory() override;

    bool IsDirectory() const override { return true; }

    FsResult Delete(bool recursive = false) override;

    FsResult AddEntry(const std::string_view rel_path, EntryBase* entry,
                      bool add_intermediate = false);
    FsResult AddEntry(const std::string_view rel_path,
                      const std::string_view host_path,
                      bool add_intermediate = false);
    FsResult DeleteEntry(const std::string_view rel_path,
                         bool recursive = false);
    FsResult GetEntry(const std::string_view rel_path, EntryBase*& out_entry);

    // Getters
    const std::map<std::string, EntryBase*>& GetEntries() const {
        return entries;
    }

  private:
    std::map<std::string, EntryBase*> entries;

    // Helpers
    template <typename CallbackEntry>
    FsResult Find(const std::string_view rel_path,
                  const std::function<FsResult(Directory*, CallbackEntry)>&
                      found_callback,
                  bool add_intermediate = false) {
        if (rel_path.empty()) {
            if constexpr (std::is_same_v<CallbackEntry, EntryBase*>) {
                return found_callback(parent, this);
            } else {
                // TODO: could be implemented through the parent
                LOG_NOT_IMPLEMENTED(Filesystem, "Empty relative path");
                return FsResult::NotImplemented;
            }
        }

        const auto slash_pos = rel_path.find('/');
        if (slash_pos == 0)
            return Find(rel_path.substr(1), found_callback, add_intermediate);

        if (slash_pos == std::string::npos) {
            auto& e = entries[std::string(rel_path)];
            return found_callback(this, e);
        } else {
            const auto sub_dir_name = rel_path.substr(0, slash_pos);
            const auto next_entry_name = rel_path.substr(slash_pos + 1);

            // Handle special names
            if (sub_dir_name == "." || sub_dir_name.empty()) {
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
            auto& e = entries[std::string(sub_dir_name)];
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
