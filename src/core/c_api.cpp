#include "core/c_api.h"

#include "core/emulation_context.hpp"

// Options
bool hydra_bool_option_get(const void* option) {
    return static_cast<const hydra::Option<bool>*>(option)->Get();
}

void hydra_bool_option_set(void* option, const bool value) {
    static_cast<hydra::Option<bool>*>(option)->Set(value);
}

int32_t hydra_i32_option_get(const void* option) {
    return static_cast<const hydra::Option<hydra::i32>*>(option)->Get();
}

void hydra_i32_option_set(void* option, const int32_t value) {
    static_cast<hydra::Option<hydra::i32>*>(option)->Set(value);
}

__uint128_t hydra_u128_option_get(const void* option) {
    return static_cast<const hydra::Option<hydra::u128>*>(option)->Get();
}

void hydra_u128_option_set(void* option, const __uint128_t value) {
    static_cast<hydra::Option<hydra::u128>*>(option)->Set(value);
}

const char* hydra_string_option_get(const void* option) {
    return static_cast<const hydra::Option<std::string>*>(option)
        ->Get()
        .c_str();
}

void hydra_string_option_set(void* option, const char* value) {
    static_cast<hydra::Option<std::string>*>(option)->Set(value);
}

const char* hydra_string_array_option_get(const void* option, uint32_t index) {
    return static_cast<const hydra::ArrayOption<std::string>*>(option)
        ->Get(index)
        .c_str();
}

void hydra_string_array_option_add(void* option, const char* value) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Add(value);
}

void hydra_string_array_option_set(void* option, uint32_t index,
                                   const char* value) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Set(index, value);
}

void hydra_string_array_option_remove(void* option, uint32_t index) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Remove(index);
}

// Config
void* hydra_config_get_game_directories() {
    return &hydra::CONFIG_INSTANCE.GetGameDirectories();
}

void* hydra_config_get_patch_directories() {
    return &hydra::CONFIG_INSTANCE.GetPatchDirectories();
}

void* hydra_config_get_sd_card_path() {
    return &hydra::CONFIG_INSTANCE.GetSdCardPath();
}

void* hydra_config_get_save_path() {
    return &hydra::CONFIG_INSTANCE.GetSavePath();
}

void* hydra_config_get_cpu_backend() {
    return &hydra::CONFIG_INSTANCE.GetCpuBackend();
}

void* hydra_config_get_gpu_renderer() {
    return &hydra::CONFIG_INSTANCE.GetGpuRenderer();
}

void* hydra_config_get_shader_backend() {
    return &hydra::CONFIG_INSTANCE.GetShaderBackend();
}

void* hydra_config_get_user_id() { return &hydra::CONFIG_INSTANCE.GetUserID(); }

void* hydra_config_get_process_args() {
    return &hydra::CONFIG_INSTANCE.GetProcessArgs();
}

void* hydra_config_get_debug_logging() {
    return &hydra::CONFIG_INSTANCE.GetDebugLogging();
}

void* hydra_config_get_stack_trace_logging() {
    return &hydra::CONFIG_INSTANCE.GetStackTraceLogging();
}

// Emulation context
void* hydra_emulation_context_create() { return new hydra::EmulationContext(); }

void hydra_emulation_context_destroy(void* ctx) {
    delete static_cast<hydra::EmulationContext*>(ctx);
}

void hydra_emulation_context_set_surface(void* ctx, void* surface) {
    static_cast<hydra::EmulationContext*>(ctx)->SetSurface(surface);
}

void hydra_emulation_context_load_rom(void* ctx, const char* rom_filename) {
    static_cast<hydra::EmulationContext*>(ctx)->LoadRom(rom_filename);
}

void hydra_emulation_context_run(void* ctx) {
    static_cast<hydra::EmulationContext*>(ctx)->Run();
}

bool hydra_emulation_context_is_running(void* ctx) {
    return static_cast<hydra::EmulationContext*>(ctx)->IsRunning();
}
