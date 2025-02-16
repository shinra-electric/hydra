#pragma once

#include "common/allocators/static_pool.hpp"
#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::NvDrv {

namespace Ioctl {
class IoctlBase;
}

class INvDrvServices : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(INvDrvServices)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void Open(REQUEST_COMMAND_PARAMS);

    // TODO: what should be the max number of ioctls?
    Allocators::StaticPool<Ioctl::IoctlBase*, 64> ioctl_pool;
};

} // namespace Hydra::Horizon::Services::NvDrv
