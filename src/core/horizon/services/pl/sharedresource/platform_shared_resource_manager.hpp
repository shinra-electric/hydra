#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

class IPlatformSharedResourceManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IPlatformSharedResourceManager)

    IPlatformSharedResourceManager();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    handle_id_t shared_memory_handle_id;

    // Commands
    void RequestLoad(REQUEST_COMMAND_PARAMS);
    void GetLoadState(REQUEST_COMMAND_PARAMS);
    void GetSize(REQUEST_COMMAND_PARAMS);
    void GetSharedMemoryAddressOffset(REQUEST_COMMAND_PARAMS);
    void GetSharedMemoryNativeHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Pl::SharedResource
