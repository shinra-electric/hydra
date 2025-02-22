#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

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

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
