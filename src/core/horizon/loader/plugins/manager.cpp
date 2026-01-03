#include "core/horizon/loader/plugins/manager.hpp"

namespace hydra::horizon::loader::plugins {

Manager::Manager() {
    for (const auto& path : CONFIG_INSTANCE.GetLoaderPaths().Get()) {
        if (!std::filesystem::exists(path)) {
            LOG_ERROR(Other, "Plugin path \"{}\" does not exist", path);
            continue;
        }
        if (!std::filesystem::is_regular_file(path)) {
            LOG_ERROR(Other, "Plugin path \"{}\" is not a file", path);
            continue;
        }

        plugins.emplace_back(path);
    }
}

Plugin* Manager::FindPluginForFormat(std::string_view format) {
    for (auto& plugin : plugins) {
        if (std::find(plugin.supported_formats.begin(),
                      plugin.supported_formats.end(),
                      format) != plugin.supported_formats.end())
            return &plugin;
    }

    return nullptr;
}

} // namespace hydra::horizon::loader::plugins
