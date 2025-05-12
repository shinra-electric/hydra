#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/audio/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

class IAudioRenderer : public ServiceBase {
  public:
    IAudioRenderer(const AudioRendererParameters& params_,
                   const usize work_buffer_size_);

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    AudioRendererParameters params;
    usize work_buffer_size;

    kernel::HandleWithId<kernel::Event> event;

    // Commands
    result_t RequestUpdate(OutBuffer<BufferAttr::MapAlias> out_buffer);
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(Stop);
    result_t QuerySystemEvent(OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace hydra::horizon::services::audio
