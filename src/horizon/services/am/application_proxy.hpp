#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IApplicationProxy : public ServiceBase {
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetCommonStateGetter(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
