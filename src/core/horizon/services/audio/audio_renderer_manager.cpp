#include "core/horizon/services/audio/audio_renderer_manager.hpp"

#include "core/horizon/services/audio/audio_renderer.hpp"

namespace Hydra::Horizon::Services::Audio {

namespace {

struct OpenAudioRendererIn {
    AudioRendererParameters params;
    u32 pad;
    u64 work_buffer_size;
    u64 aruid;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IAudioRendererManager, 0, OpenAudioRenderer, 1,
                             GetWorkBufferSize)

void IAudioRendererManager::OpenAudioRenderer(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<OpenAudioRendererIn>();

    add_service(new IAudioRenderer(in.params, in.work_buffer_size));
}

void IAudioRendererManager::GetWorkBufferSize(REQUEST_COMMAND_PARAMS) {
    const auto params = readers.reader.Read<AudioRendererParameters>();

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u64>(0x8000);
}

} // namespace Hydra::Horizon::Services::Audio
