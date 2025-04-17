#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystemProxy : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFileSystemProxy)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenFileSystem(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetCurrentProcess)
    void OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS);
    void OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS);
    void OpenPatchDataStorageByCurrentProcess(REQUEST_COMMAND_PARAMS);
    void GetGlobalAccessLogMode(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv
