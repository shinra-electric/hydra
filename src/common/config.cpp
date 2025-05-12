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

ENABLE_ENUM_FORMATTING_CASTING_AND_TOML11(hydra, CpuBackend, cpu_backend,
                                          AppleHypervisor, "Apple Hypervisor",
                                          Dynarmic, "dynarmic")

ENABLE_ENUM_FORMATTING_CASTING_AND_TOML11(hydra, GpuRenderer, gpu_renderer,
                                          Metal, "Metal")

ENABLE_ENUM_FORMATTING_CASTING_AND_TOML11(hydra, ShaderBackend, shader_backend,
                                          Msl, "MSL", Air, "AIR")

namespace hydra {

SINGLETON_DEFINE_GET_INSTANCE(Config, Other)

Config::Config() {
    SINGLETON_SET_INSTANCE(Config, Other);

#ifdef __APPLE__
    // macOS
    if (const char* home = std::getenv("HOME"))
        app_data_path =
            fmt::format("{}/Library/Application Support/" APP_NAME, home);
    else
        LOG_FATAL(Other, "Failed to find HOME path");
#elif defined(_WIN32)
    // Windows
    if (const char* app_data = std::getenv("APPDATA"))
        app_data_path = fmt::format("{}/" APP_NAME, app_data);
    else
        LOG_FATAL(Other, "Failed to find APPDATA path");
#else
    // Linux and other Unix-like systems
    if (const char* xdg_config = std::getenv("XDG_CONFIG_HOME"))
        app_data_path = fmt::format("{}/" APP_NAME, xdg_config);
    else if (const char* home = std::getenv("HOME"))
        app_data_path = fmt::format("{}/.config/" APP_NAME, home);
    else
        LOG_FATAL(Other, "Failed to find HOME path");
#endif

    // Create the app data directory
    std::filesystem::create_directories(app_data_path);

    // Load defaults
    LoadDefaults();

    // Open the config file
    std::string config_path = GetConfigPath();
    bool config_exists = std::filesystem::exists(config_path);
    if (config_exists)
        Deserialize();
}

Config::~Config() { SINGLETON_UNSET_INSTANCE(); }

void Config::LoadDefaults() {
    game_directories = GetDefaultGameDirectories();
    patch_directories = GetDefaultPatchDirectories();
    sd_card_path = GetDefaultSdCardPath();
    save_path = GetDefaultSavePath();
    cpu_backend = GetDefaultCpuBackend();
    gpu_renderer = GetDefaultGpuRenderer();
    shader_backend = GetDefaultShaderBackend();
    user_id = GetDefaultUserID();
    process_args = GetDefaultProcessArgs();
    debug_logging = GetDefaultDebugLogging();
    log_stack_trace = GetDefaultLogStackTrace();

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

            auto& patch_directories_arr = general["patch_directories"];
            patch_directories_arr = toml::array{};
            patch_directories_arr.as_array().assign(patch_directories.begin(),
                                                    patch_directories.end());

            general["sd_card_path"] = sd_card_path;
            general["save_path"] = save_path;
        }

        {
            auto& cpu = data.at("CPU");
            cpu["backend"] = cpu_backend;
        }

        {
            auto& graphics = data.at("Graphics");
            graphics["renderer"] = gpu_renderer;
            graphics["shader_backend"] = shader_backend;
        }

        {
            auto& user = data.at("User");
            user["user_id"] = user_id;
        }

        {
            auto& debug = data.at("Debug");
            debug["process_args"] = process_args;
            debug["debug_logging"] = debug_logging;
            debug["log_stack_trace"] = log_stack_trace;
        }

        config_file << toml::format(data);
        config_file.close();
    } else {
        LOG_FATAL(Other, "Failed to create config file");
    }
}

void Config::Deserialize() {
    auto data = toml::parse(GetConfigPath());

    if (data.contains("General")) {
        const auto& general = data.at("General");
        game_directories = toml::find_or<std::vector<std::string>>(
            general, "game_directories", GetDefaultGameDirectories());
        patch_directories = toml::find_or<std::vector<std::string>>(
            general, "patch_directories", GetDefaultPatchDirectories());
        sd_card_path = toml::find_or<std::string>(general, "sd_card_path",
                                                  GetDefaultSdCardPath());
        save_path = toml::find_or<std::string>(general, "save_path",
                                               GetDefaultSavePath());
    }
    if (data.contains("CPU")) {
        const auto& cpu = data.at("CPU");
        cpu_backend =
            toml::find_or<CpuBackend>(cpu, "backend", GetDefaultCpuBackend());
    }
    if (data.contains("Graphics")) {
        const auto& graphics = data.at("Graphics");
        gpu_renderer = toml::find_or<GpuRenderer>(graphics, "renderer",
                                                  GetDefaultGpuRenderer());
        shader_backend = toml::find_or<ShaderBackend>(
            graphics, "shader_backend", GetDefaultShaderBackend());
    }
    if (data.contains("User")) {
        const auto& user = data.at("User");
        user_id = toml::find_or<uuid_t>(user, "user_id", GetDefaultUserID());
    }
    if (data.contains("Debug")) {
        const auto& debug = data.at("Debug");
        process_args = toml::find_or<std::vector<std::string>>(
            debug, "process_args", GetDefaultProcessArgs());
        debug_logging = toml::find_or<bool>(debug, "debug_logging",
                                            GetDefaultDebugLogging());
        log_stack_trace = toml::find_or<bool>(debug, "log_stack_trace",
                                              GetDefaultLogStackTrace());
    }

    // Validate
    if (cpu_backend == CpuBackend::Invalid) {
        LOG_WARN(Other, "Invalid CPU backend, falling back to Dynarmic");
        cpu_backend = CpuBackend::Dynarmic;
    }

    if (gpu_renderer == GpuRenderer::Invalid) {
        LOG_WARN(Other, "Invalid GPU renderer, falling back to Metal");
        gpu_renderer = GpuRenderer::Metal;
    }

    if (shader_backend == ShaderBackend::Invalid) {
        LOG_WARN(Other, "Invalid shader backend, falling back to MSL");
        shader_backend = ShaderBackend::Msl;
    } else if (shader_backend == ShaderBackend::Air) {
        LOG_ERROR(Other, "AIR shader backend is not functional");
    }

    changed = false;
}

void Config::Log() {
    LOG_INFO(Other, "Game directories: [{}]",
             fmt::join(game_directories, ", "));
    LOG_INFO(Other, "Patch directories: [{}]",
             fmt::join(patch_directories, ", "));
    LOG_INFO(Other, "SD card path: {}", sd_card_path);
    LOG_INFO(Other, "Save path: {}", save_path);
    LOG_INFO(Other, "CPU backend: {}", cpu_backend);
    LOG_INFO(Other, "GPU renderer: {}", gpu_renderer);
    LOG_INFO(Other, "Shader backend: {}", shader_backend);
    LOG_INFO(Other, "User ID: {:032x}", user_id);
    LOG_INFO(Other, "Process arguments: {}", process_args);
    LOG_INFO(Other, "Debug logging: {}", debug_logging);
    LOG_INFO(Other, "Log stack trace: {}", log_stack_trace);
}

} // namespace hydra
