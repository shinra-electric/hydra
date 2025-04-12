#include "core/hw/tegra_x1/gpu/renderer/metal/shader.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Shader::Shader(const ShaderDescriptor& descriptor) : ShaderBase(descriptor) {
    // TODO: doesn't this copy the contents?
    std::string source;
    source.assign(descriptor.code.begin(), descriptor.code.end());

    function = CreateFunctionFromSource(Renderer::GetInstance().GetDevice(),
                                        source, "main_");
}

Shader::~Shader() { function->release(); }

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
