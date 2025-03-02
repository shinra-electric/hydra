#pragma once

#include "common/allocators/static_pool.hpp"
#include "horizon/services/nvdrv/const.hpp"
#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::NvDrv {

namespace Ioctl {
class FdBase;
}

class INvDrvServices : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(INvDrvServices)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void Open(REQUEST_COMMAND_PARAMS);
    void Ioctl(REQUEST_COMMAND_PARAMS);

    // TODO: what should be the max number of fds?
    Allocators::StaticPool<Ioctl::FdBase*, 64> fd_pool;
};

} // namespace Hydra::Horizon::Services::NvDrv
