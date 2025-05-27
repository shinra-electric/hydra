#include "core/horizon/services/audio/audio_out_manager.hpp"

#include "core/horizon/services/audio/audio_out.hpp"

namespace hydra::horizon::services::audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOutManager, 0, ListAudioOuts, 1,
                             OpenAudioOut, 3, OpenAudioOutAuto)

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
    u16 _reserved, u64 aruid,
    InBuffer<BufferAttr::MapAlias> in_device_name_buffer, u32* out_sample_rate,
    u32* out_channel_count, PcmFormat* out_pcm_format, AudioOutState* out_state,
    OutBuffer<BufferAttr::MapAlias> out_device_name_buffer) {
    return OpenAudioOutImpl(add_service, sample_rate, channel_count, _reserved,
                            aruid, *in_device_name_buffer.reader,
                            out_sample_rate, out_channel_count, out_pcm_format,
                            out_state, *out_device_name_buffer.writer);
}

result_t IAudioOutManager::OpenAudioOutAuto(
    add_service_fn_t add_service, u32 sample_rate, u16 channel_count,
    u16 _reserved, u64 aruid,
    InBuffer<BufferAttr::AutoSelect> in_device_name_buffer,
    u32* out_sample_rate, u32* out_channel_count, PcmFormat* out_pcm_format,
    AudioOutState* out_state,
    OutBuffer<BufferAttr::AutoSelect> out_device_name_buffer) {
    return OpenAudioOutImpl(add_service, sample_rate, channel_count, _reserved,
                            aruid, *in_device_name_buffer.reader,
                            out_sample_rate, out_channel_count, out_pcm_format,
                            out_state, *out_device_name_buffer.writer);
}

result_t IAudioOutManager::OpenAudioOutImpl(
    add_service_fn_t add_service, u32 sample_rate, u16 channel_count,
    u16 _reserved, u64 aruid, Reader& device_name_reader, u32* out_sample_rate,
    u32* out_channel_count, PcmFormat* out_pcm_format, AudioOutState* out_state,
    Writer& device_name_writer) {
    const auto& device_name_in = device_name_reader.ReadString();
    LOG_DEBUG(Services, "Sample rate: {}, channel count: {}, device name: {}",
              sample_rate, channel_count, device_name_in);

    add_service(new IAudioOut());

    // Out
    *out_sample_rate = sample_rate;
    *out_channel_count = channel_count;
    // HACK
    *out_pcm_format = PcmFormat::Float;
    *out_state = AudioOutState::Started;

    // TODO: correct?
    std::string device_name_out = "Hydra audio device";
    device_name_writer.WriteString(device_name_out);

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::audio
