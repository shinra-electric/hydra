#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Audio {

class IAudioOut : public ServiceBase {
  public:
    IAudioOut();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    Kernel::HandleWithId<Kernel::Event> buffer_event;

    // Commands
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(AppendAudioOutBuffer);
    result_t RegisterBufferEvent(OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace Hydra::Horizon::Services::Audio
