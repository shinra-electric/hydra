#include "horizon/services/audio/audio_renderer_manager.hpp"

#include "horizon/services/audio/audio_renderer.hpp"

namespace Hydra::Horizon::Services::Audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioRendererManager, 0, OpenAudioRenderer, 1,
                             GetWorkBufferSize)

void IAudioRendererManager::OpenAudioRenderer(REQUEST_COMMAND_PARAMS) {
    // TODO: params
    add_service(new IAudioRenderer());
}

void IAudioRendererManager::GetWorkBufferSize(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u64>(0x8000);
}

} // namespace Hydra::Horizon::Services::Audio
