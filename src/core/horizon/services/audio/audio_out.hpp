#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioOut : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioOut)

    IAudioOut();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Kernel::HandleWithId<Kernel::Event> buffer_event;

    // Commands
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(AppendAudioOutBuffer);
    void RegisterBufferEvent(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
