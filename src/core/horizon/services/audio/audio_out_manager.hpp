#pragma once

#include "core/horizon/services/audio/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

class IAudioOutManager : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t ListAudioOuts(u32* out_count,
                           OutBuffer<BufferAttr::MapAlias> out_buffer);
    result_t OpenAudioOut(add_service_fn_t add_service, u32 sample_rate,
                          u16 channel_count, u16 _reserved, u64 aruid,
                          u32* out_sample_rate, u32* out_channel_count,
                          PcmFormat* out_pcm_format, AudioOutState* out_state);
};

} // namespace hydra::horizon::services::audio
