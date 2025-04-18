#ifndef HYDRA_C_API
#define HYDRA_C_API

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Config
uint32_t hydra_config_get_game_directories_count();
const char* hydra_config_get_game_directory(uint32_t index);
void hydra_config_add_game_directory(const char* path);

uint32_t hydra_config_get_root_paths_count();
const char* hydra_config_get_root_path_guest_path(uint32_t index);
const char* hydra_config_get_root_path_host_path(uint32_t index);
bool hydra_config_get_root_path_write_access(uint32_t index);
void hydra_config_add_root_path(const char* guest_path, const char* host_path,
                                bool write_access);

int hydra_config_get_cpu_backend();
void hydra_config_set_cpu_backend(int backend);

// Emulation context
void* hydra_emulation_context_create();
void hydra_emulation_context_destroy(void* ctx);

void hydra_emulation_context_set_surface(void* ctx, void* surface);
void hydra_emulation_context_load_rom(void* ctx, const char* rom_filename);
void hydra_emulation_context_run(void* ctx);

bool hydra_emulation_context_is_running(void* ctx);

#ifdef __cplusplus
}
#endif

#endif
