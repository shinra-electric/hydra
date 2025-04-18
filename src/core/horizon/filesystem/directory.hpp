#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class Directory : public EntryBase {
  public:
    Directory() = default;
    Directory(const std::string& host_path);
    ~Directory() override;

    bool IsDirectory() const override { return true; }

    FsResult AddEntry(const std::string& rel_path, EntryBase* entry,
                      bool add_intermediate = false);
    FsResult AddEntry(const std::string& rel_path, const std::string& host_path,
                      bool add_intermediate = false);
    FsResult GetEntry(const std::string& rel_path, EntryBase*& out_entry);

    // Getters
    const std::map<std::string, EntryBase*>& GetEntries() const {
        return entries;
    }

  private:
    std::map<std::string, EntryBase*> entries;

    // Helpers
    EntryBase* GetEntryImpl(const std::string& name);
};

} // namespace Hydra::Horizon::Filesystem
