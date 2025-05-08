#include "core/c_api.h"

#include "core/emulation_context.hpp"

// Config
uint32_t hydra_config_get_game_directories_count() {
    return Hydra::Config::GetInstance().GetGameDirectories().size();
}

const char* hydra_config_get_game_directory(uint32_t index) {
    return Hydra::Config::GetInstance().GetGameDirectories()[index].c_str();
}

void hydra_config_add_game_directory(const char* path) {
    Hydra::Config::GetInstance().AddGameDirectory(path);
}

void hydra_config_remove_game_directory(uint32_t index) {
    Hydra::Config::GetInstance().RemoveGameDirectory(index);
}

uint32_t hydra_config_get_patch_directories_count() {
    return Hydra::Config::GetInstance().GetPatchDirectories().size();
}

const char* hydra_config_get_patch_directory(uint32_t index) {
    return Hydra::Config::GetInstance().GetPatchDirectories()[index].c_str();
}

void hydra_config_add_patch_directory(const char* path) {
    Hydra::Config::GetInstance().AddPatchDirectory(path);
}

void hydra_config_remove_patch_directory(uint32_t index) {
    Hydra::Config::GetInstance().RemovePatchDirectory(index);
}

const char* hydra_config_get_sd_card_path() {
    return Hydra::Config::GetInstance().GetSdCardPath().c_str();
}

void hydra_config_set_sd_card_path(const char* path) {
    Hydra::Config::GetInstance().SetSdCardPath(path);
}

int hydra_config_get_cpu_backend() {
    return static_cast<int>(Hydra::Config::GetInstance().GetCpuBackend());
}

void hydra_config_set_cpu_backend(int backend) {
    Hydra::Config::GetInstance().SetCpuBackend(
        static_cast<Hydra::CpuBackend>(backend));
}

int hydra_config_get_gpu_renderer() {
    return static_cast<int>(Hydra::Config::GetInstance().GetGpuRenderer());
}

void hydra_config_set_gpu_renderer(int renderer) {
    Hydra::Config::GetInstance().SetGpuRenderer(
        static_cast<Hydra::GpuRenderer>(renderer));
}

int hydra_config_get_shader_backend() {
    return static_cast<int>(Hydra::Config::GetInstance().GetShaderBackend());
}

void hydra_config_set_shader_backend(int backend) {
    Hydra::Config::GetInstance().SetShaderBackend(
        static_cast<Hydra::ShaderBackend>(backend));
}

uint32_t hydra_config_get_process_args_count() {
    return Hydra::Config::GetInstance().GetProcessArgs().size();
}

const char* hydra_config_get_process_arg(uint32_t index) {
    return Hydra::Config::GetInstance().GetProcessArgs()[index].c_str();
}

void hydra_config_add_process_arg(const char* arg) {
    Hydra::Config::GetInstance().AddProcessArg(arg);
}

void hydra_config_remove_process_arg(uint32_t index) {
    Hydra::Config::GetInstance().RemoveProcessArg(index);
}

bool hydra_config_is_debug_logging_enabled() {
    return Hydra::Config::GetInstance().IsDebugLoggingEnabled();
}

void hydra_config_set_debug_logging(bool enabled) {
    Hydra::Config::GetInstance().SetDebugLogging(enabled);
}

bool hydra_config_is_log_stack_trace_enabled() {
    return Hydra::Config::GetInstance().IsLogStackTraceEnabled();
}

void hydra_config_set_log_stack_trace(bool enabled) {
    Hydra::Config::GetInstance().SetLogStackTrace(enabled);
}

// Emulation context
void* hydra_emulation_context_create() { return new Hydra::EmulationContext(); }

void hydra_emulation_context_destroy(void* ctx) {
    delete static_cast<Hydra::EmulationContext*>(ctx);
}

void hydra_emulation_context_set_surface(void* ctx, void* surface) {
    static_cast<Hydra::EmulationContext*>(ctx)->SetSurface(surface);
}

void hydra_emulation_context_load_rom(void* ctx, const char* rom_filename) {
    static_cast<Hydra::EmulationContext*>(ctx)->LoadRom(rom_filename);
}

void hydra_emulation_context_run(void* ctx) {
    static_cast<Hydra::EmulationContext*>(ctx)->Run();
}

bool hydra_emulation_context_is_running(void* ctx) {
    return static_cast<Hydra::EmulationContext*>(ctx)->IsRunning();
}
