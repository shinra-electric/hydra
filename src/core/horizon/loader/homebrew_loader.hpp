#pragma once

#include "core/horizon/loader/nro_loader.hpp"

namespace hydra::horizon::loader {

class HomebrewLoader : public LoaderBase {
  public:
    HomebrewLoader(filesystem::FileBase* file_);

    void LoadProcess(kernel::Process* process) override;

  private:
    filesystem::FileBase* file;
    NroLoader nro_loader;

    void TryLoadAssetSection(filesystem::FileBase* file);
};

} // namespace hydra::horizon::loader
