#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/const.hpp"

#define OPERAND_TABLE_ENTRY(e, value, operand)                                 \
    case value:                                                                \
        return e::operand;

#define DEFINE_OPERAND_TABLE(name, e, mask, ...)                               \
    inline e GetOperand##name(u64 inst) {                                      \
        switch (inst & mask) {                                                 \
            FOR_EACH_1_2(OPERAND_TABLE_ENTRY, e, __VA_ARGS__)                  \
        default:                                                               \
            LOG_ERROR(ShaderDecompiler,                                        \
                      "Couldn't extract opernad from instruction 0x{:016x}",   \
                      inst);                                                   \
            return e::Invalid;                                                 \
        }                                                                      \
    }

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

DEFINE_OPERAND_TABLE(Eff0_0, LoadStoreMode, 0x0001800000000000,
                     0x0000000000000000, B32, 0x0000800000000000, B64,
                     0x0001000000000000, B96, 0x0001800000000000, B128)

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
