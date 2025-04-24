#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class DisplayServiceBase : public Kernel::ServiceBase {
  protected:
    // Commands
    void CreateStrayLayer(REQUEST_COMMAND_PARAMS);
    void SetLayerVisibility(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::ViSrv
