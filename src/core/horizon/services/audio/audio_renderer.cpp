#include "core/horizon/services/audio/audio_renderer.hpp"

namespace Hydra::Horizon::Services::Audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioRenderer, 4, RequestUpdate, 5, Start, 7,
                             QuerySystemEvent)

IAudioRenderer::IAudioRenderer() : event(new Event()) {}

void IAudioRenderer::QuerySystemEvent(REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(event.id);
}

} // namespace Hydra::Horizon::Services::Audio
