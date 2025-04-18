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

uint32_t hydra_config_get_root_paths_count() {
    return Hydra::Config::GetInstance().GetRootPaths().size();
}

const char* hydra_config_get_root_path_guest_path(uint32_t index) {
    return Hydra::Config::GetInstance()
        .GetRootPaths()[index]
        .guest_path.c_str();
}

const char* hydra_config_get_root_path_host_path(uint32_t index) {
    return Hydra::Config::GetInstance().GetRootPaths()[index].host_path.c_str();
}

bool hydra_config_get_root_path_write_access(uint32_t index) {
    return Hydra::Config::GetInstance().GetRootPaths()[index].write_access;
}

void hydra_config_add_root_path(const char* guest_path, const char* host_path,
                                bool write_access) {
    Hydra::Config::GetInstance().AddRootPath(guest_path, host_path,
                                             write_access);
}

int hydra_config_get_cpu_backend() {
    return static_cast<int>(Hydra::Config::GetInstance().GetCpuBackend());
}

void hydra_config_set_cpu_backend(int backend) {
    Hydra::Config::GetInstance().SetCpuBackend(
        static_cast<Hydra::CpuBackend>(backend));
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
