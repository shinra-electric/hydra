#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class Directory : public EntryBase {
  public:
    Directory(Directory* parent_) : parent{parent_} {}
    Directory(Directory* parent_, const std::string& host_path);
    ~Directory() override;

    bool IsDirectory() const override { return true; }

    void AddEntry(EntryBase* entry, const std::string& rel_path);
    void AddEntry(const std::string& host_path, const std::string& rel_path);
    EntryBase* GetEntry(const std::string& rel_path);

    // Getters
    EntryBase* GetEntry(const std::string& name) const {
        return entries.at(name);
    }

  private:
    Directory* parent;

    std::map<std::string, EntryBase*> entries;

    // Helpers
    EntryBase* GetEntryImpl(const std::string& name);
};

} // namespace Hydra::Horizon::Filesystem
