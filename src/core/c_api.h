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
void hydra_config_remove_game_directory(uint32_t index);

const char* hydra_config_get_sd_card_path();
void hydra_config_set_sd_card_path(const char* path);

int hydra_config_get_cpu_backend();
void hydra_config_set_cpu_backend(int backend);

int hydra_config_get_gpu_renderer();
void hydra_config_set_gpu_renderer(int renderer);

int hydra_config_get_shader_backend();
void hydra_config_set_shader_backend(int backend);

uint32_t hydra_config_get_process_args_count();
const char* hydra_config_get_process_arg(uint32_t index);
void hydra_config_add_process_arg(const char* arg);
void hydra_config_remove_process_arg(uint32_t index);

bool hydra_config_is_debug_logging_enabled();
void hydra_config_set_debug_logging(bool enabled);

bool hydra_config_is_log_stack_trace_enabled();
void hydra_config_set_log_stack_trace(bool enabled);

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
