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

enum class LDOperand : u64 {
    Invalid,
    B32,
    B64,
    B96,
    B128,
};

DEFINE_OPERAND_TABLE(Eff0_0, LDOperand, 0x0001800000000000, 0x0000000000000000,
                     B32, 0x0000800000000000, B64, 0x0001000000000000, B96,
                     0x0001800000000000, B128)

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::LDOperand,
                       Invalid, "invalid", B32, "b32", B64, "b64", B96, "b96",
                       B128, "b128")
