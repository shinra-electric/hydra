#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IAppletResource : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAppletResource)

    IAppletResource();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    HandleId shared_memory_handle_id;

    // Commands
    void GetSharedMemoryHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Hid
