#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/shader_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Shader final : public ShaderBase {
  public:
    Shader(const ShaderDescriptor& descriptor);
    ~Shader() override;

    // Getters
    MTL::Function* GetFunction() const { return function; }

  private:
    MTL::Function* function;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
