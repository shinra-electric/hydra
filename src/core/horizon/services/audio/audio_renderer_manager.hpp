#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioRendererManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioRendererManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenAudioRenderer(REQUEST_COMMAND_PARAMS);
    void GetWorkBufferSize(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
