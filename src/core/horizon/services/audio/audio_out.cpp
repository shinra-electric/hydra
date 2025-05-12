#include "core/horizon/services/audio/audio_out.hpp"

namespace Hydra::Horizon::Services::Audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOut, 1, Start, 3, AppendAudioOutBuffer, 4,
                             RegisterBufferEvent)

IAudioOut::IAudioOut() : buffer_event(new Kernel::Event()) {}

result_t IAudioOut::RegisterBufferEvent(OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = buffer_event.id;
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Audio
