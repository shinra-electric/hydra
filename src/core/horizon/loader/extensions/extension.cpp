#include "core/horizon/loader/extensions/extension.hpp"

namespace hydra::horizon::loader::extensions {

Extension::Extension(const std::string& path) {
    library = dlopen(path.data(), RTLD_LAZY);
    ASSERT_THROWING(library, Loader, Error::LoadFailed,
                    "Failed to load extension at path {}: {}", path, dlerror());

    // API version
    ASSERT_THROWING(GetApiVersion() == 1, Loader, Error::InvalidApiVersion,
                    "Invalid API version");

    // Context
    context = CreateContext({}); // TODO: options
    ASSERT_THROWING(context, Loader, Error::ContextCreationFailed,
                    "Failed to create context");

    // Info
    std::array<u8, 1024> buffer;
    name = Query(context, api::QueryType::Name, buffer);
    display_version = Query(context, api::QueryType::DisplayVersion, buffer);
    supported_formats = split<std::string>(
        Query(context, api::QueryType::SupportedFormats, buffer), ',');

    LOG_INFO(
        Loader,
        "Loaded extension \"{}\" (version: {}, formats: {}) at path \"{}\"",
        name, display_version, supported_formats, path);
}

Extension::~Extension() {
    // TODO: destroy context
    dlclose(library);
}

} // namespace hydra::horizon::loader::extensions
