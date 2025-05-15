#include "core/horizon/services/audio/audio_renderer.hpp"

namespace hydra::horizon::services::audio {

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
    u32 _unknown;
    u32 render_info_size;
    u32 _reserved[4];
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
    u32 _padding[3];
};

struct VoiceInfoOut {
    u64 played_sample_count;
    u32 num_wavebufs_consumed;
    u32 voice_drops_count;
};

enum class EffectState : u8 {
    Enabled = 3,
    Disabled = 4,
};

struct EffectInfoOutV1 {
    EffectState state;
    u8 _reserved[15];
};

struct SinkInfoOut {
    u32 last_written_offset;
    u32 _padding;
    u64 _reserved[3];
};

struct ErrorInfo {
    result_t result;
    u32 _padding;
    u64 extra_error_info;
};

struct BehaviorInfoOut {
    ErrorInfo error_infos[10];
    u32 error_info_count;
    u32 _reserved[3];
};

struct RenderInfoOut {
    u64 elapsed_frame_count;
    u64 _reserved;
};

struct PerformanceInfoOut {
    u32 history_size;
    u32 _reserved[3];
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IAudioRenderer, 4, RequestUpdate, 5, Start, 6,
                             Stop, 7, QuerySystemEvent)

// TODO: autoclear event?
IAudioRenderer::IAudioRenderer(const AudioRendererParameters& params_,
                               const usize work_buffer_size_)
    : params{params_}, work_buffer_size{work_buffer_size_},
      event(new kernel::Event()) {}

result_t
IAudioRenderer::RequestUpdate(InBuffer<BufferAttr::MapAlias> in_buffer,
                              OutBuffer<BufferAttr::MapAlias> out_buffer,
                              OutBuffer<BufferAttr::MapAlias> out_perf_buffer) {
    LOG_FUNC_STUBBED(Services);

    auto& writer = *out_buffer.writer;

    // Header
    // TODO: correct?
    auto header = writer.WritePtr<UpdateDataHeader>();
    *header = {
        .revision = params.revision,
        .total_size = sizeof(UpdateDataHeader),
    };

    // Mempools
    header->mempools_size =
        (params.effect_count + params.voice_count * 4) * sizeof(MemPoolInfoOut);
    header->total_size += header->mempools_size;
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
    header->voices_size = params.voice_count * sizeof(VoiceInfoOut);
    header->total_size += header->voices_size;
    for (u32 i = 0; i < params.voice_count; i++) {
        const VoiceInfoOut voice{
            .played_sample_count = 0,
            .num_wavebufs_consumed = 1,
            .voice_drops_count = 0,
        };
        writer.Write(voice);
    }

    // Effects
    if (false) {
        // header->effects_size = TODO;
        // TODO
    } else {
        header->effects_size = params.effect_count * sizeof(EffectInfoOutV1);
        for (u32 i = 0; i < params.effect_count; i++) {
            writer.Write<EffectInfoOutV1>({
                .state = EffectState::Enabled,
            });
        }
    }
    header->total_size += header->effects_size;

    // Sinks
    header->sinks_size = params.sink_count * sizeof(SinkInfoOut);
    header->total_size += header->sinks_size;
    for (u32 i = 0; i < params.sink_count; i++) {
        writer.Write<SinkInfoOut>({
            .last_written_offset = 0,
        });
    }

    // Behavior
    header->behavior_size = sizeof(BehaviorInfoOut);
    header->total_size += header->behavior_size;
    writer.Write<BehaviorInfoOut>({
        .error_info_count = 0,
    });

    // Render info
    header->render_info_size = sizeof(RenderInfoOut);
    header->total_size += header->render_info_size;
    writer.Write<RenderInfoOut>({
        .elapsed_frame_count = 0,
    });

    // Performance
    header->perfmgr_size = sizeof(PerformanceInfoOut);
    header->total_size += header->perfmgr_size;
    // HACK
    if (out_perf_buffer.writer) {
        out_perf_buffer.writer->Write<PerformanceInfoOut>({
            .history_size = 0,
        });
    }

    return RESULT_SUCCESS;
}

result_t
IAudioRenderer::QuerySystemEvent(OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = event.id;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::audio
