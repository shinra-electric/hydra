#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioOutManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioOutManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenAudioOut(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
