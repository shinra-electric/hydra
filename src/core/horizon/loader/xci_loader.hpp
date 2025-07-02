#pragma once

#include "core/horizon/loader/nsp_loader.hpp"

namespace hydra::horizon::loader {

class XciLoader : public LoaderBase {
  public:
    XciLoader(filesystem::FileBase* file);

    u64 GetTitleID() const override { return nsp_loader->GetTitleID(); }

    std::optional<kernel::ProcessParams> LoadProcess() override {
        return nsp_loader->LoadProcess();
    }

  private:
    NspLoader* nsp_loader{nullptr};
};

} // namespace hydra::horizon::loader
