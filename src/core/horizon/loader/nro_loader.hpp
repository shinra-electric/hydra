#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NroLoader : public LoaderBase {
  public:
    NroLoader(filesystem::FileBase* file_);

    void LoadProcess(kernel::Process* process) override;

  private:
    filesystem::FileBase* file;
    u64 size;
    u32 text_offset;
    u32 bss_size;

    void TryLoadAssetSection(filesystem::FileBase* file);

  public:
    GETTER(size, GetSize);
};

} // namespace hydra::horizon::loader
