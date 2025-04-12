#pragma once

#include "common/allocators/static_pool.hpp"
#include "core/horizon/services/nvdrv/const.hpp"
#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::NvDrv {

namespace Ioctl {
class FdBase;
}

constexpr usize MAX_FD_COUNT = 256;

class INvDrvServices : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(INvDrvServices)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void Open(REQUEST_COMMAND_PARAMS);
    void Ioctl(REQUEST_COMMAND_PARAMS);
    void Initialize(REQUEST_COMMAND_PARAMS);
    void QueryEvent(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetAruid)

    // TODO: what should be the max number of fds?
    static Allocators::StaticPool<Ioctl::FdBase*, MAX_FD_COUNT> fd_pool;
};

} // namespace Hydra::Horizon::Services::NvDrv
