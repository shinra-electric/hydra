#include "core/horizon/services/audio/audio_renderer.hpp"

namespace Hydra::Horizon::Services::Audio {

namespace {

struct UpdateDataHeader {
    u32 revision;
    u32 behavior_size;
    u32 mempools_size;
    u32 voices_size;
    u32 channels_size;
    u32 effects_size;
    u32 mixes_size;
    u32 sinks_size;
    u32 perfmgr_size;
    u32 padding[6];
    u32 total_size;
};

enum class MemPoolState : u32 {
    Invalid,
    New,
    RequestDetach,
    Detached,
    RequestAttach,
    Attached,
    Released,
};

struct MemPoolInfoOut {
    MemPoolState new_state;
    u32 padding2[3];
};

struct VoiceInfoOut {
    u64 played_sample_count;
    u32 num_wavebufs_consumed;
    u32 voice_drops_count;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IAudioRenderer, 4, RequestUpdate, 5, Start, 6,
                             Stop, 7, QuerySystemEvent)

IAudioRenderer::IAudioRenderer(const AudioRendererParameters& params_,
                               const usize work_buffer_size_)
    : params{params_}, work_buffer_size{work_buffer_size_},
      event(new Kernel::Event()) {}

result_t
IAudioRenderer::RequestUpdate(OutBuffer<BufferAttr::MapAlias> out_buffer) {
    LOG_FUNC_STUBBED(HorizonServices);

    auto& writer = *out_buffer.writer;

    // Header
    // TODO: correct?
    const UpdateDataHeader header{
        .mempools_size =
            static_cast<u32>(params.effect_count + params.voice_count * 4),
        .voices_size = static_cast<u32>(params.voice_count),
    };
    writer.Write(header);

    // Mempools
    for (u32 i = 0; i < params.effect_count; i++) {
        const MemPoolInfoOut mempool{
            .new_state = MemPoolState::Released, // HACK
        };
        writer.Write(mempool);
    }

    for (u32 i = 0; i < params.voice_count; i++) {
        for (u32 j = 0; j < 4; j++) {
            const MemPoolInfoOut mempool{
                .new_state = MemPoolState::Released, // HACK
            };
            writer.Write(mempool);
        }
    }

    // Voices
    for (u32 i = 0; i < params.voice_count; i++) {
        const VoiceInfoOut voice{
            .played_sample_count = 0,
            .num_wavebufs_consumed = 1,
            .voice_drops_count = 0,
        };
        writer.Write(voice);
    }

    return RESULT_SUCCESS;
}

result_t
IAudioRenderer::QuerySystemEvent(OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = event.id;
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Audio
