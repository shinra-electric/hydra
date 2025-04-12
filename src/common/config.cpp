#include "common/config.hpp"
#include "common/logging/log.hpp"

namespace Hydra {

SINGLETON_DEFINE_GET_INSTANCE(Config, Other, "Config")

Config::Config() {
    SINGLETON_SET_INSTANCE(Other, "Config");

#ifdef __APPLE__
    // macOS
    if (const char* home = std::getenv("HOME"))
        app_data_path =
            fmt::format("{}/Library/Application Support/" APP_NAME, home);
    else
        LOG_ERROR(Other, "Failed to find HOME path");
#elif defined(_WIN32)
    // Windows
    if (const char* app_data = std::getenv("APPDATA"))
        app_data_path = fmt::format("{}/" APP_NAME, app_data);
    else
        LOG_ERROR(Other, "Failed to find APPDATA path");
#else
    // Linux and other Unix-like systems
    if (const char* xdg_config = std::getenv("XDG_CONFIG_HOME"))
        app_data_path = fmt::format("{}/" APP_NAME, xdg_config);
    else if (const char* home = std::getenv("HOME"))
        app_data_path = fmt::format("{}/.config/" APP_NAME, home);
    else
        LOG_ERROR(Other, "Failed to find HOME path");
#endif

    // Create the app data directory
    std::filesystem::create_directories(app_data_path);

    // Open the config file
    std::string config_path = fmt::format("{}/config.toml", app_data_path);
    bool config_exists = std::filesystem::exists(config_path);
    if (config_exists) {
        std::ifstream config_file(config_path);

        // TODO: load config values
        std::string data((std::istreambuf_iterator<char>(config_file)),
                         std::istreambuf_iterator<char>());
        LOG_DEBUG(Other, "Config: {}", data);

        // HACK
        LoadDefaults();

        config_file.close();
    } else {
        // Write default config values
        std::ofstream config_file(config_path);
        if (config_file.is_open()) {
            LoadDefaults();

            // TODO: save config
            config_file << "TODO";

            config_file.close();
        } else {
            LOG_ERROR(Other, "Failed to create config file");
        }
    }
}

Config::~Config() { SINGLETON_UNSET_INSTANCE(); }

void Config::LoadDefaults() { cpu_backend = CpuBackend::Dynarmic; }

} // namespace Hydra
