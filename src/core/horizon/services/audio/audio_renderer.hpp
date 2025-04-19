#pragma once

#include "core/horizon/services/audio/const.hpp"
#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioRenderer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAudioRenderer)

    IAudioRenderer(const AudioRendererParameters& params_,
                   const usize work_buffer_size_);

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    AudioRendererParameters params;
    usize work_buffer_size;

    KernelHandleWithId<Event> event;

    // Commands
    void RequestUpdate(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(Stop);
    void QuerySystemEvent(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Audio
