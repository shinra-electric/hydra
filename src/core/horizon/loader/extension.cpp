#include "core/horizon/loader/extension.hpp"

namespace hydra::horizon::loader {

Extension::Extension(std::string_view path) {
    library = dlopen(path.data(), RTLD_LAZY);
    if (!library) {
        // TODO: return error
        LOG_FATAL(Loader, "Failed to load extension at path {}: {}", path,
                  dlerror());
    }

    // Info
    std::array<u8, 1024> buffer;
    name = Query(api::QueryType::Name, buffer);
    display_version = Query(api::QueryType::DisplayVersion, buffer);
    supported_formats = split<std::string>(
        Query(api::QueryType::SupportedFormats, buffer), ',');

    LOG_INFO(
        Loader,
        "Loaded extension \"{}\" (version: {}, formats: {}) at path \"{}\"",
        name, display_version, supported_formats, path);
}

Extension::~Extension() { dlclose(library); }

} // namespace hydra::horizon::loader
