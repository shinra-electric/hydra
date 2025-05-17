#include "core/horizon/services/audio/audio_out_manager.hpp"

#include "core/horizon/services/audio/audio_out.hpp"

namespace hydra::horizon::services::audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOutManager, 0, ListAudioOuts, 1,
                             OpenAudioOut)

result_t
IAudioOutManager::ListAudioOuts(u32* out_count,
                                OutBuffer<BufferAttr::MapAlias> out_buffer) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_count = 0;
    return RESULT_SUCCESS;
}

result_t IAudioOutManager::OpenAudioOut(
    add_service_fn_t add_service, u32 sample_rate, u16 channel_count,
    u16 _reserved, u64 aruid, u32* out_sample_rate, u32* out_channel_count,
    PcmFormat* out_pcm_format, AudioOutState* out_state) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Sample rate: {}, channel count: {}", sample_rate,
              channel_count);

    // TODO: name in and out

    add_service(new IAudioOut());

    // Out
    *out_sample_rate = sample_rate;
    *out_channel_count = channel_count;
    // HACK
    *out_pcm_format = PcmFormat::Float;
    *out_state = AudioOutState::Started;

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::audio
