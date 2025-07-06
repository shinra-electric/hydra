#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/cfg.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {

enum class LastStatement {
    None,
    Exit,
    Break,
    Continue,
};

}

ENABLE_ENUM_FORMATTING(
    hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer::LastStatement,
    None, "none", Exit, "exit", Break, "break", Continue, "continue")

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {

struct CfgNode {
    virtual ~CfgNode() = default;

    virtual bool IsSameAs(const CfgNode* other) const = 0;

    // Debug
    virtual void Log(const u32 indent = 0) const = 0;
};

struct CfgStructuredNodeWithEdge : public CfgNode {
    CfgNode* node;
    CfgNode* branch_target;

    CfgStructuredNodeWithEdge(CfgNode* node_, CfgNode* branch_target_)
        : node{node_}, branch_target{branch_target_} {}

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_with_edge =
                dynamic_cast<const CfgStructuredNodeWithEdge*>(other)) {
            return node->IsSameAs(other_with_edge->node) &&
                   branch_target->IsSameAs(other_with_edge->branch_target);
        }

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "Structured node with edge",
                  PASS_INDENT(indent));
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Node:", PASS_INDENT(indent + 1));
        node->Log(indent + 2);
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Branch target:", PASS_INDENT(indent + 1));
        branch_target->Log(indent + 2);
    }
};

struct CfgCodeBlock : public CfgNode {
    label_t label;
    LastStatement last_statement;

    CfgCodeBlock(const label_t label_,
                 LastStatement last_statement_ = LastStatement::None)
        : label{label_}, last_statement{last_statement_} {}

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_code_block = dynamic_cast<const CfgCodeBlock*>(other))
            return label == other_code_block->label;

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Code block:", PASS_INDENT(indent));
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "Label: {}",
                  PASS_INDENT(indent + 1), label);
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "Last statement: {}",
                  PASS_INDENT(indent + 1), last_statement);
    }
};

struct CfgBlock : public CfgNode {
    std::vector<CfgNode*> nodes;

    CfgBlock(const std::vector<CfgNode*>& nodes_) {
        for (const auto node : nodes_) {
            if (auto block_node = dynamic_cast<CfgBlock*>(node))
                nodes.insert(nodes.end(), block_node->nodes.begin(),
                             block_node->nodes.end());
            else
                nodes.push_back(node);
        }
    }

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_block = dynamic_cast<const CfgBlock*>(other)) {
            if (nodes.size() != other_block->nodes.size())
                return false;

            for (u32 i = 0; i < nodes.size(); i++) {
                if (!nodes[i]->IsSameAs(other_block->nodes[i]))
                    return false;
            }

            return true;
        }

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "Block:", PASS_INDENT(indent));
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Nodes:", PASS_INDENT(indent + 1));
        for (const auto& node : nodes)
            node->Log(indent + 2);
    }
};

struct CfgIfBlock : public CfgNode {
    ir::Value cond;
    CfgNode* then_block;

    CfgIfBlock(const ir::Value& cond_, CfgNode* then_block_)
        : cond{cond_}, then_block{then_block_} {}

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_if_block = dynamic_cast<const CfgIfBlock*>(other)) {
            return cond == other_if_block->cond &&
                   then_block->IsSameAs(other_if_block->then_block);
        }

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "If block:", PASS_INDENT(indent));
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "if {}", PASS_INDENT(indent + 1),
                  cond);
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Then block:", PASS_INDENT(indent + 1));
        then_block->Log(indent + 2);
    }
};

struct CfgIfElseBlock : public CfgNode {
    ir::Value cond;
    CfgNode* then_block;
    CfgNode* else_block;

    CfgIfElseBlock(const ir::Value& cond_, CfgNode* then_block_,
                   CfgNode* else_block_)
        : cond{cond_}, then_block{then_block_}, else_block{else_block_} {}

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_if_else_block =
                dynamic_cast<const CfgIfElseBlock*>(other)) {
            return cond == other_if_else_block->cond &&
                   then_block->IsSameAs(other_if_else_block->then_block) &&
                   else_block->IsSameAs(other_if_else_block->else_block);
        }

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "If-else block:", PASS_INDENT(indent));
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "if {}", PASS_INDENT(indent + 1),
                  cond);
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Then block:", PASS_INDENT(indent + 1));
        then_block->Log(indent + 2);
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Else block:", PASS_INDENT(indent + 1));
        else_block->Log(indent + 2);
    }
};

struct CfgWhileBlock : public CfgNode {
    bool is_do_while;
    ir::Value cond;
    CfgNode* body_block;

    CfgWhileBlock(const bool is_do_while_, const ir::Value& cond_,
                  CfgNode* body_block_)
        : is_do_while{is_do_while_}, cond{cond_}, body_block{body_block_} {}

    bool IsSameAs(const CfgNode* other) const override {
        if (auto other_if_block = dynamic_cast<const CfgWhileBlock*>(other)) {
            return is_do_while == other_if_block->is_do_while &&
                   cond == other_if_block->cond &&
                   body_block->IsSameAs(other_if_block->body_block);
        }

        return false;
    }

    void Log(const u32 indent = 0) const override {
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "{} block:", PASS_INDENT(indent),
                  (is_do_while ? "Do-While" : "While"));
        LOG_DEBUG(ShaderDecompiler, INDENT_FMT "while {}",
                  PASS_INDENT(indent + 1), cond);
        LOG_DEBUG(ShaderDecompiler,
                  INDENT_FMT "Body block:", PASS_INDENT(indent + 1));
        body_block->Log(indent + 2);
    }

    GETTER(is_do_while, IsDoWhile);
};

CfgBlock* Structurize(const CfgBasicBlock* entry_bb);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer
