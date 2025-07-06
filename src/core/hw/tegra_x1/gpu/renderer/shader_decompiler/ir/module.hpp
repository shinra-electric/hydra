#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/function.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

class Module {
  public:
    Function& GetFunction(const std::string& name) { return functions[name]; }

  private:
    std::map<std::string, Function> functions;

  public:
    CONST_REF_GETTER(functions, GetFunctions);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir
