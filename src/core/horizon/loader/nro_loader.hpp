#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NroLoader : public LoaderBase {
  public:
    NroLoader(filesystem::FileBase* file_);

    std::optional<kernel::ProcessParams> LoadProcess() override;

  private:
    filesystem::FileBase* file;
    u32 text_offset;
    usize bss_size;
};

} // namespace hydra::horizon::loader
