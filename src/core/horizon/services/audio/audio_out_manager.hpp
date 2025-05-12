#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

class IAudioOutManager : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t OpenAudioOut(add_service_fn_t add_service, u32 sample_rate,
                          u16 channel_count, u16 reserved, u64 aruid,
                          u32* out_sample_rate, u32* out_channel_count,
                          u32* out_pcm_format, u32* out_state);
};

} // namespace hydra::horizon::services::audio
