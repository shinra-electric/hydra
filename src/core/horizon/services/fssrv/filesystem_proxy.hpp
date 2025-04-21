#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystemProxy : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFileSystemProxy)

    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenFileSystem(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetCurrentProcess)
    void OpenFileSystemWithIdObsolete(REQUEST_COMMAND_PARAMS);
    void OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS);
    void CreateSaveDataFileSystem(REQUEST_COMMAND_PARAMS);
    void OpenSaveDataFileSystem(REQUEST_COMMAND_PARAMS);
    void OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS);
    void OpenPatchDataStorageByCurrentProcess(REQUEST_COMMAND_PARAMS);
    void GetGlobalAccessLogMode(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv
