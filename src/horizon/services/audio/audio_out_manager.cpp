#include "horizon/services/audio/audio_out_manager.hpp"

#include "horizon/services/audio/audio_out.hpp"

namespace Hydra::Horizon::Services::Audio {

namespace {

struct OpenAudioOutIn {
    u32 sample_rate;
    u16 channel_count;
    u16 reserved;
    u64 aruid;
};

struct OpenAudioOutOut {
    u32 sample_rate;
    u32 channel_count;
    u32 pcm_format;
    u32 state;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IAudioOutManager, 1, OpenAudioOut)

void IAudioOutManager::OpenAudioOut(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<OpenAudioOutIn>();
    LOG_DEBUG(HorizonServices, "Sample rate: {}, channel count: {}",
              in.sample_rate, in.channel_count);

    // TODO: name in and out

    add_service(new IAudioOut());

    // Out
    // TODO: correct?
    OpenAudioOutOut out{
        .sample_rate = in.sample_rate, .channel_count = in.channel_count,
        // TODO: pcm_format
        // TODO: state
    };
    writers.writer.Write(out);
}

} // namespace Hydra::Horizon::Services::Audio
