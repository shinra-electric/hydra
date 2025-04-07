#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioRendererManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioRendererManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Audio
