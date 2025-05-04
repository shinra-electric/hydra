#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator {

enum class ResultCode {
    None,
    Branch,
    BranchConditional,
    SyncPoint,
    EndBlock,

    Error,
};

struct Result {
    ResultCode code;
    union {
        u32 target;
    };
};

class IteratorBase {
  public:
    IteratorBase(Reader code_reader_) : code_reader{code_reader_} {}
    virtual ~IteratorBase() = default;

    virtual void Iterate(ObserverBase* observer) = 0;

  protected:
    Result ParseNextInstruction(ObserverBase* observer);

    void Jump(u32 target) { code_reader.Seek(target * sizeof(instruction_t)); }

  private:
    Reader code_reader;

    Result ParseNextInstructionImpl(ObserverBase* observer, const u32 pc,
                                    const instruction_t inst);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator
