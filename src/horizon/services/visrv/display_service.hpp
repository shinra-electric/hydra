#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class DisplayServiceBase : public ServiceBase {
  protected:
    // Commands
    void CmdCreateStrayLayer(REQUEST_PARAMS_WITH_RESULT);
};

} // namespace Hydra::Horizon::Services::ViSrv
