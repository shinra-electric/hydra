#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioRenderer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioRenderer)

    IAudioRenderer();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    KernelHandleWithId<Event> event;

    // Commands
    STUB_REQUEST_COMMAND(RequestUpdate);
    STUB_REQUEST_COMMAND(Start);
    void QuerySystemEvent(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
