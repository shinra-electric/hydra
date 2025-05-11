#include "core/horizon/services/audio/audio_renderer_manager.hpp"

namespace Hydra::Horizon::Services::Audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioRendererManager, 0, OpenAudioRenderer, 1,
                             GetWorkBufferSize)

result_t IAudioRendererManager::OpenAudioRenderer(
    add_service_fn_t add_service, aligned<AudioRendererParameters, 56> params,
    u64 work_buffer_size, u64 aruid) {
    add_service(new IAudioRenderer(params, work_buffer_size));
    return RESULT_SUCCESS;
}

result_t
IAudioRendererManager::GetWorkBufferSize(AudioRendererParameters params,
                                         u64* out_size) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_size = 0x8000;
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Audio
