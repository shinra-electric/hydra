#pragma once

#include "horizon/filesystem/directory.hpp"

namespace Hydra::Horizon::Filesystem {

class File;
class Directory;

class Filesystem {
  public:
    static Filesystem& GetInstance();

    Filesystem();
    ~Filesystem();

    void AddEntry(EntryBase* entry, const std::string& path);
    EntryBase* GetEntry(const std::string& path);

    File* GetFile(const std::string& path);
    Directory* GetDirectory(const std::string& path);

    // Getters
    Directory& GetRoot() { return root; }

  private:
    Directory root;
};

} // namespace Hydra::Horizon::Filesystem
