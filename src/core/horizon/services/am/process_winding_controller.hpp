#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

struct AppletProcessLaunchReason {
    u8 flag;
    u16 _unknown_x1;
    u8 _unknown_x3;
};

class IProcessWindingController : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetLaunchReason(AppletProcessLaunchReason* out_reason);
};

} // namespace hydra::horizon::services::am
