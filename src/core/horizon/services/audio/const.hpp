#pragma once

#include "common/common.hpp"

namespace hydra::horizon::services::audio {

enum class PcmFormat : u32 {
    Invalid = 0,
    Int8 = 1,
    Int16 = 2,
    Int24 = 3,
    Int32 = 4,
    Float = 5,
    Adpcm = 6,
};

enum class AudioOutState : u32 {
    Started = 0,
    Stopped = 1,
};

struct AudioRendererParameters {
    i32 sample_rate;
    i32 sample_count;
    i32 mix_buffer_count;
    i32 submix_count;
    i32 voice_count;
    i32 sink_count;
    i32 effect_count;
    i32 unk1;
    u8 unk2;
    u8 padding1[3];
    i32 splitter_count;
    i32 unk3;
    i32 unk4;
    u32 revision;
};

} // namespace hydra::horizon::services::audio
