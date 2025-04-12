#pragma once

#include "core/horizon/services/fssrv/storage.hpp"

namespace Hydra::Horizon::Services::Fssrv {

enum class FileFlags {
    None = 0,
    Read = BIT(0),
    Write = BIT(1),
    Append = BIT(2),
};

ENABLE_ENUM_BITMASK_OPERATORS(FileFlags)

class IFile : public IStorage {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFile)

    IFile(const std::string& path, FileFlags flags_)
        : IStorage(path), flags{flags_} {}

  private:
    FileFlags flags;

    // Commands
    void Read(REQUEST_COMMAND_PARAMS) override;
};

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FLAGS_FORMATTING(Hydra::Horizon::Services::Fssrv::FileFlags, Read,
                             "read", Write, "write", Append, "append")
