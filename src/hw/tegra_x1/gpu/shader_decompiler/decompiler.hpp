#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class Decompiler final {
  public:
    Decompiler() = default;
    ~Decompiler() = default;

    void Decompile(const u32* code, Renderer::ShaderType type,
                   std::vector<u8>& out_code);

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
