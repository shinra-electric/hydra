#pragma once

#include "core/horizon/filesystem/const.hpp"
#include "core/horizon/services/fssrv/storage.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFile : public IStorage {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFile)

    IFile(Filesystem::FileBase* file, Filesystem::FileOpenFlags flags)
        : IStorage(file, flags) {}

  private:
    // Commands
    void Read(REQUEST_COMMAND_PARAMS) override;
    void Write(REQUEST_COMMAND_PARAMS) override;
};

} // namespace Hydra::Horizon::Services::Fssrv
