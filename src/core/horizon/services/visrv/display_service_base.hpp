#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class DisplayServiceBase : public ServiceBase {
  protected:
    // Commands
    void CreateStrayLayer(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::ViSrv
