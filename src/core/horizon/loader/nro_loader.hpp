#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

struct NroSection {
    u32 offset;
    u32 size;
};

class NroLoader : public LoaderBase {
  public:
    enum class Error {
        InvalidMagic,
    };

    NroLoader(filesystem::IFile* file_, const bool is_entry_point_);

    void LoadProcess(kernel::Process* process) override;

    vaddr_t GetEntryPoint() const;

  private:
    filesystem::IFile* file;
    const bool is_entry_point;

    u64 size;
    NroSection sections[3];

    uptr executable_ptr{invalid<uptr>()};
    vaddr_t executable_base{invalid<vaddr_t>()};
    usize executable_size{0};

    void TryLoadAssetSection(filesystem::IFile* file);

  public:
    GETTER(size, GetSize);
    GETTER(executable_ptr, GetExecutablePtr);
    GETTER(executable_base, GetExecutableBase);
    GETTER(executable_size, GetExecutableSize);
};

} // namespace hydra::horizon::loader
