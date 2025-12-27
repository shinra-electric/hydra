#pragma once

#include "core/horizon/loader/nro_loader.hpp"

namespace hydra::horizon::loader {

class HomebrewLoader : public LoaderBase {
  public:
    HomebrewLoader(filesystem::IFile* file_);

    void LoadProcess(kernel::Process* process) override;

  private:
    filesystem::IFile* file;
    NroLoader nro_loader;

    void TryLoadAssetSection(filesystem::IFile* file);
};

} // namespace hydra::horizon::loader
