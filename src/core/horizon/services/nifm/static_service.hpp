#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Nifm {

class IStaticService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStaticService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateGeneralServiceOld(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Nifm
