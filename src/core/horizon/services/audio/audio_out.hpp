#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioOut : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioOut)

    IAudioOut();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    KernelHandleWithId<Event> buffer_event;

    // Commands
    STUB_REQUEST_COMMAND(Start);
    void RegisterBufferEvent(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
