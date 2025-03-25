#pragma once

#include "horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class Directory : public EntryBase {
  public:
    Directory() = default;
    Directory(const std::string& host_path);
    ~Directory() override;

    void AddEntry(EntryBase* entry, const std::string& rel_path);
    EntryBase* GetEntry(const std::string& rel_path);

    // Getters
    EntryBase* GetEntry(const std::string& name) const {
        return entries.at(name);
    }

  private:
    std::map<std::string, EntryBase*> entries;

    // Helpers
    EntryBase* GetEntryImpl(const std::string& name);
};

} // namespace Hydra::Horizon::Filesystem
