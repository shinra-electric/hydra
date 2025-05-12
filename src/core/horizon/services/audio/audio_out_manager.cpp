#include "core/horizon/services/audio/audio_out_manager.hpp"

#include "core/horizon/services/audio/audio_out.hpp"

namespace hydra::horizon::services::audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOutManager, 1, OpenAudioOut)

result_t IAudioOutManager::OpenAudioOut(add_service_fn_t add_service,
                                        u32 sample_rate, u16 channel_count,
                                        u16 reserved, u64 aruid,
                                        u32* out_sample_rate,
                                        u32* out_channel_count,
                                        u32* out_pcm_format, u32* out_state) {
    LOG_DEBUG(Services, "Sample rate: {}, channel count: {}",
              sample_rate, channel_count);

    // TODO: name in and out

    add_service(new IAudioOut());

    // Out
    // TODO: correct?
    *out_sample_rate = sample_rate;
    *out_channel_count = channel_count;
    // TODO: pcm_format
    // TODO: state

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::audio
