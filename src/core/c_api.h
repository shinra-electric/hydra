#ifndef HYDRA_C_API
#define HYDRA_C_API

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Options
bool hydra_bool_option_get(const void* option);
void hydra_bool_option_set(void* option, const bool value);

int32_t hydra_i32_option_get(const void* option);
void hydra_i32_option_set(void* option, const int32_t value);

__uint128_t hydra_u128_option_get(const void* option);
void hydra_u128_option_set(void* option, const __uint128_t value);

const char* hydra_string_option_get(const void* option);
void hydra_string_option_set(void* option, const char* value);

const char* hydra_string_array_option_get(const void* option, uint32_t index);
void hydra_string_array_option_add(void* option, const char* value);
void hydra_string_array_option_set(void* option, uint32_t index,
                                   const char* value);
void hydra_string_array_option_remove(void* option, uint32_t index);

// Config
void* hydra_config_get_game_directories();
void* hydra_config_get_patch_directories();
void* hydra_config_get_sd_card_path();
void* hydra_config_get_save_path();
void* hydra_config_get_cpu_backend();
void* hydra_config_get_gpu_renderer();
void* hydra_config_get_shader_backend();
void* hydra_config_get_user_id();
void* hydra_config_get_process_args();
void* hydra_config_get_debug_logging();
void* hydra_config_get_stack_trace_logging();

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
