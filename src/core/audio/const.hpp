#pragma once

namespace hydra::audio {

enum class PcmFormat : u32 {
    Invalid = 0,
    Int8 = 1,
    Int16 = 2,
    Int24 = 3,
    Int32 = 4,
    Float = 5,
    Adpcm = 6,
};

}
