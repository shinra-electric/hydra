#pragma once

#include "core/hw/tegra_x1/gpu/engines/inline_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

struct RegsInline_ {
    RegsInline regs_inline;
};

class Inline : public EngineWithRegsBase<RegsInline_>, public InlineBase {
  public:
    void Method(u32 method, u32 arg) override;

  private:
    // Methods
    DEFINE_INLINE_ENGINE_METHODS;
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
