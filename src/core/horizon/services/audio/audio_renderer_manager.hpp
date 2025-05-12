#pragma once

#include "core/horizon/services/audio/audio_renderer.hpp"
#include "core/horizon/services/audio/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

class IAudioRendererManager : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t OpenAudioRenderer(add_service_fn_t add_service,
                               aligned<AudioRendererParameters, 56> params,
                               u64 work_buffer_size, u64 aruid);
    result_t GetWorkBufferSize(AudioRendererParameters params, u64* out_size);
};

} // namespace hydra::horizon::services::audio
