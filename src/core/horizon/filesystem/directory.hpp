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
        const auto entry_name = rel_path.substr(0, slash_pos);

        // TODO: scrap the whole entry_ref thing

        // Get the entry
        EntryBase* entry;
        EntryBase** entry_ref = nullptr;
        Directory* p;
        if (entry_name == "." || entry_name.empty()) {
            entry = this;
            p = parent;
        } else if (entry_name == "..") {
            if (parent) {
                entry = parent;
                p = entry->GetParent();
            } else {
                return FsResult::DoesNotExist;
            }
        } else {
            entry_ref = &entries[std::string(entry_name)];
            entry = *entry_ref;
            p = this;
        }

        if (slash_pos == std::string::npos) {
            // TODO: ugh
            return found_callback(p, entry_ref ? *entry_ref : entry);
        } else {
            const auto next_entry_name = rel_path.substr(slash_pos + 1);

            // Regular subdirectory
            if (!entry) {
                if (add_intermediate) {
                    // TODO: ugh
                    *entry_ref = new Directory();
                    entry = *entry_ref;
                    entry->SetParent(this);
                } else {
                    return FsResult::IntermediateDirectoryDoesNotExist;
                }
            }

            auto sub_dir = dynamic_cast<Directory*>(entry);
            if (!sub_dir)
                return FsResult::NotADirectory;

            return sub_dir->Find(next_entry_name, found_callback,
                                 add_intermediate);
        }
    }
};

} // namespace hydra::horizon::filesystem
