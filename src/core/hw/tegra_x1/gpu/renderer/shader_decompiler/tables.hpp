#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

#define OPERAND_TABLE_ENTRY(e, value, operand)                                 \
    case value:                                                                \
        return e::operand;

#define DEFINE_OPERAND_TABLE(name, e, mask, ...)                               \
    inline e get_operand_##name(u64 inst) {                                    \
        switch (inst & mask) {                                                 \
            FOR_EACH_1_2(OPERAND_TABLE_ENTRY, e, __VA_ARGS__)                  \
        default:                                                               \
            LOG_ERROR(ShaderDecompiler,                                        \
                      "Couldn't extract opernad from instruction 0x{:016x}",   \
                      inst);                                                   \
            return e::Invalid;                                                 \
        }                                                                      \
    }

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

DEFINE_OPERAND_TABLE(eff0_0, LoadStoreMode, 0x0001800000000000,
                     0x0000000000000000, B32, 0x0000800000000000, B64,
                     0x0001000000000000, B96, 0x0001800000000000, B128)

DEFINE_OPERAND_TABLE(5080_0, MathFunc, 0x0000000000f00000, 0x0000000000000000,
                     Cos, 0x0000000000100000, Sin, 0x0000000000200000, Ex2,
                     0x0000000000300000, Lg2, 0x0000000000400000, Rcp,
                     0x0000000000500000, Rsq, 0x0000000000600000, Rcp64h,
                     0x0000000000700000, Rsq64h, 0x0000000000800000, Sqrt)

DEFINE_OPERAND_TABLE(5bb0_0, ComparisonOperator, 0x000f000000000000,
                     0x0000000000000000, F, 0x0001000000000000, Less,
                     0x0002000000000000, Equal, 0x0003000000000000, LessEqual,
                     0x0004000000000000, Greater, 0x0005000000000000, NotEqual,
                     0x0006000000000000, GreaterEqual, 0x0007000000000000, Num,
                     0x0008000000000000, Nan, 0x0009000000000000, LessU,
                     0x000a000000000000, EqualU, 0x000b000000000000, LessEqualU,
                     0x000c000000000000, GreaterU, 0x000d000000000000,
                     NotEqualU, 0x000e000000000000, GreaterEqualU,
                     0x000f000000000000, T)

DEFINE_OPERAND_TABLE(5bb0_1, BinaryOperator, 0x0000600000000000,
                     0x0000000000000000, And, 0x0000200000000000, Or,
                     0x0000400000000000, Xor)

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
