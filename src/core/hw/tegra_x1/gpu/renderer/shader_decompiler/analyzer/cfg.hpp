#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

enum class CfgNodeEdgeType {
    Branch,
    BranchConditional,
    Exit,
};

struct CfgNode;

struct CfgNodeEdge {
    CfgNodeEdgeType type;
    union {
        struct {
            CfgNode* target;
        } branch;
        struct {
            CfgNode* target_true;
            CfgNode* target_false;
        } branch_conditional;
    };
};

struct CfgNode {
    range<u32> code_range;
    u32 return_sync_point;
    CfgNodeEdge edge;
};

enum class CfgNodeStatus {
    Unvisited,
    Visited,
    Finished,
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::
                           Analyzer::CfgNodeEdgeType,
                       Branch, "branch", BranchConditional,
                       "branch conditional", Exit, "exit")

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

class CFG {
  public:
    CfgNode& GetNode(u32 label) { return GetNodeImpl(label).first; }

    CfgNode& VisitNode(u32 label) {
        auto& node_ = GetNodeImpl(label);
        auto& status = node_.second;
        ASSERT_DEBUG(status == CfgNodeStatus::Unvisited, ShaderDecompiler,
                     "Node 0x{:x} already visited", label);

        status = CfgNodeStatus::Visited;
        return node_.first;
    }

    void FinishNode(u32 label, const u32 end, const CfgNodeEdge& edge) {
        auto& node_ = nodes[label];
        auto& node = node_.first;
        auto& status = node_.second;
        ASSERT_DEBUG(status == CfgNodeStatus::Visited, ShaderDecompiler,
                     "Node 0x{:x} finished without being visited", label);

        node.code_range.end = end;
        node.edge = edge;
        status = CfgNodeStatus::Finished;
    }

    void FinishNode(const CfgNode& node, const u32 end,
                    const CfgNodeEdge& edge) {
        FinishNode(node.code_range.begin, end, edge);
    }

    // Debug
    void LogNodes() const {
        for (const auto& [label, node_] : nodes) {
            LOG_DEBUG(ShaderDecompiler, "Label: {}", label);

            const auto status = node_.second;
            if (status == CfgNodeStatus::Unvisited) {
                LOG_DEBUG(ShaderDecompiler, "UNVISITED");
                continue;
            } else if (status == CfgNodeStatus::Visited) {
                LOG_DEBUG(ShaderDecompiler, "VISITED");
                continue;
            }

            auto& node = node_.first;
            LOG_DEBUG(ShaderDecompiler, "\tCode: {}", node.code_range);
            if (node.return_sync_point != invalid<u32>())
                LOG_DEBUG(ShaderDecompiler, "\tReturn Sync Point: {}",
                          node.return_sync_point);

            const auto& edge = node.edge;
            LOG_DEBUG(ShaderDecompiler, "\tEdge: {}", edge.type);
            switch (edge.type) {
            case CfgNodeEdgeType::Branch:
                LOG_DEBUG(ShaderDecompiler, "\t\tTarget: {}",
                          edge.branch.target->code_range.begin);
                break;
            case CfgNodeEdgeType::BranchConditional:
                LOG_DEBUG(
                    ShaderDecompiler, "\t\tTarget True: {}",
                    edge.branch_conditional.target_true->code_range.begin);
                LOG_DEBUG(
                    ShaderDecompiler, "\t\tTarget False: {}",
                    edge.branch_conditional.target_false->code_range.begin);
                break;
            case CfgNodeEdgeType::Exit:
                break;
            }
        }
    }

  private:
    std::map<u32, std::pair<CfgNode, CfgNodeStatus>> nodes;

    std::pair<CfgNode, CfgNodeStatus>& GetNodeImpl(u32 label) {
        auto& node_ = nodes[label];
        auto& node = node_.first;
        const auto& status = node_.second;
        if (status == CfgNodeStatus::Unvisited) {
            node.code_range = range(label);
        }

        return node_;
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
