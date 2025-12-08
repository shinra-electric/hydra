#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/builder.hpp"

// TODO: ignore on release
#define COMMENT_IMPL(log_level, f_comment, f_log, ...)                         \
    {                                                                          \
        /* TODO: comments */                                                   \
        /*BUILDER.OpDebugComment(fmt::format(f_comment                         \
         * PASS_VA_ARGS(__VA_ARGS__)));*/                                      \
        LOG_##log_level(ShaderDecompiler, f_log PASS_VA_ARGS(__VA_ARGS__));    \
    }
#define COMMENT(f, ...) COMMENT_IMPL(DEBUG, f, f, __VA_ARGS__)
#define COMMENT_NOT_IMPLEMENTED(f, ...)                                        \
    COMMENT_IMPL(NOT_IMPLEMENTED, f " (NOT IMPLEMENTED)", f, __VA_ARGS__)

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

struct DecoderContext {
    const DecompilerContext& decomp_context;
    Reader code_reader;
    ir::Builder& builder;
};

inline ir::Value neg_if(ir::Builder& builder, const ir::Value& value,
                        bool neg) {
    return neg ? builder.OpNeg(value) : value;
}

inline ir::Value abs_if(ir::Builder& builder, const ir::Value& value,
                        bool abs) {
    return abs ? builder.OpAbs(value) : value;
}

inline ir::Value abs_neg_if(ir::Builder& builder, const ir::Value& value,
                            bool abs, bool neg) {
    return neg_if(builder, abs_if(builder, value, abs), neg);
}

inline ir::Value not_if(ir::Builder& builder, const ir::Value& value,
                        bool not_) {
    return not_ ? builder.OpNot(value) : value;
}

inline bool HandlePredCond(ir::Builder& builder, pred_t pred, bool pred_inv) {
    if (pred == PT) {
        if (!pred_inv) { // Always
        } else {         // Never
            // TODO: implement
            LOG_FATAL(ShaderDecompiler, "Never condition encountered");
        }

        return false;
    } else { // Conditional
        builder.OpBeginIf(
            {not_if(builder, ir::Value::Predicate(pred), pred_inv)});
        return true;
    }
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
