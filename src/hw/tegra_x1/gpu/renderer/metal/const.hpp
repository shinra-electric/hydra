#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

// Cast from const char* to NS::String*
inline NS::String* ToNSString(const char* str) {
    return NS::String::string(str, NS::ASCIIStringEncoding);
}

// Cast from std::string to NS::String*
inline NS::String* ToNSString(const std::string& str) {
    return ToNSString(str.c_str());
}

// Cast from const char* to NS::URL*
inline NS::URL* ToNSURL(const char* str) {
    return NS::URL::fileURLWithPath(ToNSString(str));
}

// Cast from std::string to NS::URL*
inline NS::URL* ToNSURL(const std::string& str) { return ToNSURL(str.c_str()); }

inline NS::String* GetLabel(const std::string& label, const void* identifier) {
    return ToNSString(label + " (" +
                      std::to_string(reinterpret_cast<uintptr_t>(identifier)) +
                      ")");
}

inline MTL::Library* CreateLibraryFromSource(MTL::Device* device,
                                             const std::string& source) {
    NS::Error* error;
    MTL::Library* library =
        device->newLibrary(ToNSString(source), nullptr, &error);
    if (error) {
        LOG_ERROR(GPU, "Failed to create library: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
        return nullptr;
    }

    return library;
}

inline MTL::Function* CreateFunctionFromSource(MTL::Device* device,
                                               const std::string& source,
                                               const std::string& name) {
    auto library = CreateLibraryFromSource(device, source);
    auto function = library->newFunction(ToNSString(name));
    library->release();

    return function;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
