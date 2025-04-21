#include "common/config.hpp"

#include <fmt/ranges.h>
#include <toml.hpp>

#include "common/logging/log.hpp"

#define TOML11_CONVERSION_TOML_TO_ENUM_CASE(e, val, n)                         \
    if (v.as_string() == n)                                                    \
        return e::val;

#define TOML11_CONVERSION_ENUM_TO_TOML_CASE(e, val, n)                         \
    case e::val:                                                               \
        return toml::value(n);

#define TOML11_DEFINE_CONVERSION_ENUM(e, ...)                                  \
    namespace toml {                                                           \
    template <> struct from<e> {                                               \
        template <typename TC> static e from_toml(const basic_value<TC>& v) {  \
            FOR_EACH_1_2(TOML11_CONVERSION_TOML_TO_ENUM_CASE, e, __VA_ARGS__)  \
            return e::Invalid;                                                 \
        }                                                                      \
    };                                                                         \
    template <> struct into<e> {                                               \
        template <typename TC>                                                 \
        static basic_value<TC> into_toml(const e& obj) {                       \
            switch (obj) {                                                     \
                FOR_EACH_1_2(TOML11_CONVERSION_ENUM_TO_TOML_CASE, e,           \
                             __VA_ARGS__)                                      \
            default:                                                           \
                return toml::value("invalid");                                 \
            }                                                                  \
        }                                                                      \
    };                                                                         \
    } /* toml */

#define ENABLE_ENUM_FORMATTING_CASTING_AND_TOML11(namespc, e, e_lower_case,    \
                                                  ...)                         \
    ENABLE_ENUM_FORMATTING_AND_CASTING(namespc, e, e_lower_case, __VA_ARGS__)  \
    TOML11_DEFINE_CONVERSION_ENUM(namespc::e, __VA_ARGS__)

#define ENABLE_STRUCT_FORMATTING_AND_TOML11(s, ...)                            \
    ENABLE_STRUCT_FORMATTING(s, __VA_ARGS__)                                   \
    TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(s, __VA_ARGS__)

ENABLE_STRUCT_FORMATTING_AND_TOML11(Hydra::RootPath, guest_path, host_path)

ENABLE_ENUM_FORMATTING_CASTING_AND_TOML11(Hydra, CpuBackend, cpu_backend,
                                          AppleHypervisor, "Apple Hypervisor",
                                          Dynarmic, "dynarmic")

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
    std::string config_path = GetConfigPath();
    bool config_exists = std::filesystem::exists(config_path);
    if (config_exists) {
        Deserialize();
    } else {
        // Load defaults
        LoadDefaults();
    }

    // Log
    LOG_INFO(Other, "Game directories: [{}]",
             fmt::join(game_directories, ", "));
    LOG_INFO(Other, "SD card path: {}", sd_card_path);
    // TODO: uncomment
    // LOG_INFO(Other, "Root directories: [{}]", fmt::join(root_directories, ",
    // "));
    LOG_INFO(Other, "CPU backend: {}", cpu_backend);
    LOG_INFO(Other, "Debug logging: {}", debug_logging);
}

Config::~Config() { SINGLETON_UNSET_INSTANCE(); }

void Config::LoadDefaults() {
    game_directories = GetDefaultGameDirectories();
    sd_card_path = GetDefaultSdCardPath();
    root_paths = GetDefaultRootPaths();
    cpu_backend = GetDefaultCpuBackend();
    debug_logging = GetDefaultDebugLogging();

    changed = true;
}

void Config::Serialize() {
    if (!changed)
        return;

    std::ofstream config_file(GetConfigPath());
    if (config_file.is_open()) {
        toml::value data(toml::table{
            {"General", toml::table{}},
            {"CPU", toml::table{}},
        });

        {
            auto& general = data.at("General");

            auto& game_directories_arr = general["game_directories"];
            game_directories_arr = toml::array{};
            game_directories_arr.as_array().assign(game_directories.begin(),
                                                   game_directories.end());

            general["sd_card_path"] = sd_card_path;

            auto& root_paths_arr = general["root_paths"];
            root_paths_arr = toml::array{};
            root_paths_arr.as_array().assign(root_paths.begin(),
                                             root_paths.end());
        }

        {
            auto& cpu = data.at("CPU");
            cpu["backend"] = cpu_backend;
        }

        {
            auto& debug = data.at("Debug");
            debug["debug_logging"] = debug_logging;
        }

        config_file << toml::format(data);
        config_file.close();
    } else {
        LOG_ERROR(Other, "Failed to create config file");
    }
}

void Config::Deserialize() {
    auto data = toml::parse(GetConfigPath());

    if (data.contains("General")) {
        const auto& general = data.at("General");
        game_directories = toml::find_or<std::vector<std::string>>(
            general, "game_directories", GetDefaultGameDirectories());
        sd_card_path = toml::find_or<std::string>(general, "sd_card_path",
                                                  GetDefaultSdCardPath());
        root_paths = toml::find_or<std::vector<RootPath>>(
            general, "root_paths", GetDefaultRootPaths());
    }
    if (data.contains("CPU")) {
        const auto& cpu = data.at("CPU");
        cpu_backend =
            toml::find_or<CpuBackend>(cpu, "backend", GetDefaultCpuBackend());
    }
    if (data.contains("Debug")) {
        const auto& debug = data.at("Debug");
        debug_logging = toml::find_or<bool>(debug, "debug_logging",
                                            GetDefaultDebugLogging());
    }

    // Validate
    if (cpu_backend == CpuBackend::Invalid) {
        LOG_WARNING(Other, "Invalid CPU backend, using Dynarmic");
        cpu_backend = CpuBackend::Dynarmic;
    }
}

} // namespace Hydra
