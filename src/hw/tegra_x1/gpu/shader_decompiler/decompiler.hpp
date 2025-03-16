#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class Decompiler final {
  public:
    Decompiler() = default;
    ~Decompiler() = default;

    void Decompile(Reader& code_reader, Renderer::ShaderType type,
                   std::vector<u8>& out_code);

  private:
    void ParseInstruction(u64 inst);
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
