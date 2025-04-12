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
    HandleId shared_memory_handle_id;

    // Commands
    void GetSharedMemoryNativeHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Pl::SharedResource
