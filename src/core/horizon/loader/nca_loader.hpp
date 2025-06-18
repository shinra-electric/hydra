#pragma once

#include "core/horizon/filesystem/content_archive.hpp"
#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NcaLoader : public LoaderBase {
  public:
    NcaLoader(filesystem::FileBase* file) : content_archive(file) {}

    std::optional<kernel::ProcessParams> LoadProcess() override;

  private:
    filesystem::ContentArchive content_archive;

    // Helpers
    std::optional<kernel::ProcessParams> LoadCode(filesystem::Directory* dir);
};

} // namespace hydra::horizon::loader
