#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/structured_iterator.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/lang_builder_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/structurizer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang {

void StructuredIterator::Iterate(ObserverBase* observer) {
    auto builder = dynamic_cast<LangBuilderBase*>(observer);
    ASSERT_DEBUG(builder, ShaderDecompiler,
                 "Observer must be of type LangBuilderBase");
    IterateImpl(builder, root_block);
}

void StructuredIterator::IterateImpl(LangBuilderBase* builder,
                                     const CfgNode* node) {
#define PARSE_NEXT_INSTRUCTION()                                               \
    {                                                                          \
        const auto res = ParseNextInstruction(builder);                        \
        switch (res.code) {                                                    \
        case ResultCode::None:                                                 \
        case ResultCode::SyncPoint:                                            \
        case ResultCode::EndBlock:                                             \
            break;                                                             \
        case ResultCode::Branch:                                               \
        case ResultCode::BranchConditional:                                    \
            LOG_ERROR(ShaderDecompiler, "Unexpected branch");                  \
            break;                                                             \
        case ResultCode::Error:                                                \
            LOG_ERROR(ShaderDecompiler, "Error");                              \
            return;                                                            \
        }                                                                      \
    }

    if (auto code_block = dynamic_cast<const CfgCodeBlock*>(node)) {
        builder->EnterScopeEmpty();
        Jump(builder, code_block->code_range.begin);
        for (u32 i = 0; i < code_block->code_range.GetSize(); i++)
            PARSE_NEXT_INSTRUCTION();
        builder->ExitScopeEmpty();
    } else if (auto block = dynamic_cast<const CfgBlock*>(node)) {
        builder->EnterScopeEmpty();
        for (const auto block_node : block->nodes)
            IterateImpl(builder, block_node);
        builder->ExitScopeEmpty();
    } else if (auto if_else_block = dynamic_cast<const CfgIfElseBlock*>(node)) {
        builder->SetPredCond(if_else_block->pred_cond);
        IterateImpl(builder, if_else_block->then_block);
        builder->EnterScopeTemp("else");
        IterateImpl(builder, if_else_block->else_block);
    } else {
        LOG_ERROR(ShaderDecompiler, "Invalid structured node");
    }

#undef PARSE_NEXT_INSTRUCTION
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang
