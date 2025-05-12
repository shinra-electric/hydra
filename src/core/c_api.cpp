#include "core/c_api.h"

#include "core/emulation_context.hpp"

// Config
uint32_t hydra_config_get_game_directories_count() {
    return hydra::Config::GetInstance().GetGameDirectories().size();
}

const char* hydra_config_get_game_directory(uint32_t index) {
    return hydra::Config::GetInstance().GetGameDirectories()[index].c_str();
}

void hydra_config_add_game_directory(const char* path) {
    hydra::Config::GetInstance().AddGameDirectory(path);
}

void hydra_config_remove_game_directory(uint32_t index) {
    hydra::Config::GetInstance().RemoveGameDirectory(index);
}

uint32_t hydra_config_get_patch_directories_count() {
    return hydra::Config::GetInstance().GetPatchDirectories().size();
}

const char* hydra_config_get_patch_directory(uint32_t index) {
    return hydra::Config::GetInstance().GetPatchDirectories()[index].c_str();
}

void hydra_config_add_patch_directory(const char* path) {
    hydra::Config::GetInstance().AddPatchDirectory(path);
}

void hydra_config_remove_patch_directory(uint32_t index) {
    hydra::Config::GetInstance().RemovePatchDirectory(index);
}

const char* hydra_config_get_sd_card_path() {
    return hydra::Config::GetInstance().GetSdCardPath().c_str();
}

void hydra_config_set_sd_card_path(const char* path) {
    hydra::Config::GetInstance().SetSdCardPath(path);
}

int hydra_config_get_cpu_backend() {
    return static_cast<int>(hydra::Config::GetInstance().GetCpuBackend());
}

void hydra_config_set_cpu_backend(int backend) {
    hydra::Config::GetInstance().SetCpuBackend(
        static_cast<hydra::CpuBackend>(backend));
}

int hydra_config_get_gpu_renderer() {
    return static_cast<int>(hydra::Config::GetInstance().GetGpuRenderer());
}

void hydra_config_set_gpu_renderer(int renderer) {
    hydra::Config::GetInstance().SetGpuRenderer(
        static_cast<hydra::GpuRenderer>(renderer));
}

int hydra_config_get_shader_backend() {
    return static_cast<int>(hydra::Config::GetInstance().GetShaderBackend());
}

void hydra_config_set_shader_backend(int backend) {
    hydra::Config::GetInstance().SetShaderBackend(
        static_cast<hydra::ShaderBackend>(backend));
}

uint32_t hydra_config_get_process_args_count() {
    return hydra::Config::GetInstance().GetProcessArgs().size();
}

const char* hydra_config_get_process_arg(uint32_t index) {
    return hydra::Config::GetInstance().GetProcessArgs()[index].c_str();
}

void hydra_config_add_process_arg(const char* arg) {
    hydra::Config::GetInstance().AddProcessArg(arg);
}

void hydra_config_remove_process_arg(uint32_t index) {
    hydra::Config::GetInstance().RemoveProcessArg(index);
}

bool hydra_config_is_debug_logging_enabled() {
    return hydra::Config::GetInstance().IsDebugLoggingEnabled();
}

void hydra_config_set_debug_logging(bool enabled) {
    hydra::Config::GetInstance().SetDebugLogging(enabled);
}

bool hydra_config_is_log_stack_trace_enabled() {
    return hydra::Config::GetInstance().IsLogStackTraceEnabled();
}

void hydra_config_set_log_stack_trace(bool enabled) {
    hydra::Config::GetInstance().SetLogStackTrace(enabled);
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
