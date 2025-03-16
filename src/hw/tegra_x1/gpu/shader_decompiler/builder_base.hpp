#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class BuilderBase {
  public:
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

    // Operations
    virtual void OpMove(reg_t dst, u32 value) = 0;
    virtual void OpLoad(reg_t dst, reg_t src, u64 imm) = 0;
    virtual void OpStore(reg_t src, reg_t dst, u64 imm) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
