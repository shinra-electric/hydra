#include "core/horizon/loader/extension_manager.hpp"

namespace hydra::horizon::loader {

ExtensionManager::ExtensionManager() {
    for (const auto& path : CONFIG_INSTANCE.GetLoaderPaths().Get()) {
        if (!std::filesystem::exists(path)) {
            LOG_ERROR(Other, "Extension path \"{}\" does not exist", path);
            continue;
        }
        if (!std::filesystem::is_regular_file(path)) {
            LOG_ERROR(Other, "Extension path \"{}\" is not a file", path);
            continue;
        }

        extensions.emplace_back(path);
    }
}

Extension* ExtensionManager::FindExtensionForFormat(std::string_view format) {
    for (auto& extension : extensions) {
        if (std::find(extension.supported_formats.begin(),
                      extension.supported_formats.end(),
                      format) != extension.supported_formats.end())
            return &extension;
    }

    return nullptr;
}

} // namespace hydra::horizon::loader
