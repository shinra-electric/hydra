#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class ShaderBase {
  public:
    virtual ~ShaderBase() = default;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
