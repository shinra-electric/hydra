#include "core/horizon/services/audio/audio_out.hpp"

namespace Hydra::Horizon::Services::Audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOut, 1, Start, 3, AppendAudioOutBuffer, 4,
                             RegisterBufferEvent)

IAudioOut::IAudioOut() : buffer_event(new Kernel::Event()) {}

void IAudioOut::RegisterBufferEvent(REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(buffer_event.id);
}

} // namespace Hydra::Horizon::Services::Audio
