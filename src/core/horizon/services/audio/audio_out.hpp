#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

class IAudioOut : public ServiceBase {
  public:
    IAudioOut();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> buffer_event;

    // Commands
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(AppendAudioOutBuffer);
    result_t RegisterBufferEvent(OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace hydra::horizon::services::audio
