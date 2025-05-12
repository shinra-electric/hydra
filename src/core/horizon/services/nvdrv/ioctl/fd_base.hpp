#pragma once

#include "core/horizon/services/nvdrv/ioctl/const.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class FdBase {
  public:
    virtual NvResult Ioctl(IoctlContext& context, u32 type, u32 nr) = 0;
    virtual NvResult QueryEvent(u32 event_id_u32, handle_id_t& out_handle_id) {
        LOG_WARN(HorizonServices, "Unknown event id {}", event_id_u32);
        return NvResult::NotSupported;
    }
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
