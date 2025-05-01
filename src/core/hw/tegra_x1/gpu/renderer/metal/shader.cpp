#include "core/hw/tegra_x1/gpu/renderer/metal/shader.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Shader::Shader(const ShaderDescriptor& descriptor) : ShaderBase(descriptor) {
    MTL::Library* library;
    switch (descriptor.backend) {
    case ShaderBackend::Msl: {
        // TODO: doesn't this copy the contents?
        std::string source;
        source.assign(descriptor.code.begin(), descriptor.code.end());

        library = CreateLibraryFromSource(Renderer::GetInstance().GetDevice(),
                                          source);
        break;
    }
    case ShaderBackend::Air: {
        auto dispatch_data =
            dispatch_data_create(descriptor.code.data(), descriptor.code.size(),
                                 dispatch_get_global_queue(0, 0),
                                 ^{
                                 });

        NS::Error* error;
        library = Renderer::GetInstance().GetDevice()->newLibrary(dispatch_data,
                                                                  &error);
        if (error) {
            LOG_ERROR(MetalRenderer, "Failed to create Metal library: {}",
                      error->localizedDescription()->utf8String());
            error->release(); // TODO: release?
        }
        break;
    }
    default:
        // TODO: log the backend
        LOG_FATAL(MetalRenderer,
                  "Unsupported shader backend for the Metal renderer");
        break;
    }

    function = library->newFunction(ToNSString("main_"));
    library->release();
}

Shader::~Shader() { function->release(); }

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
