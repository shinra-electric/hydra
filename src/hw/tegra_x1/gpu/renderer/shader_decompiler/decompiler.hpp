#pragma once

#include "hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {
struct GuestShaderState;
}

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

class ObserverBase;
class BuilderBase;

class Decompiler final {
  public:
    Decompiler() = default;
    ~Decompiler() = default;

    void Decompile(Reader& code_reader, const ShaderType type,
                   const GuestShaderState& state, std::vector<u8>& out_code,
                   ResourceMapping& out_resource_mapping);

  private:
    bool ParseInstruction(ObserverBase* observer, u64 inst);
    void Parse(ObserverBase* observer, Reader& code_reader);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
