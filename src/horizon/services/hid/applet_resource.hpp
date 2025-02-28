#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IAppletResource : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IAppletResource)

    IAppletResource();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Handle shared_memory_handle;

    // Commands
    void GetSharedMemoryHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Hid
