#ifndef HYDRA_C_API
#define HYDRA_C_API

#ifdef __cplusplus
extern "C" {
#endif

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
