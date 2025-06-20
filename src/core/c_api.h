#ifndef HYDRA_C_API
#define HYDRA_C_API

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Enums
typedef enum : uint32_t {
    HYDRA_CPU_BACKEND_INVALID = 0,

    HYDRA_CPU_BACKEND_APPLE_HYPERVISOR,
    HYDRA_CPU_BACKEND_DYNARMIC,
} HydraCpuBackend;

typedef enum : uint32_t {
    HYDRA_GPU_RENDERER_INVALID = 0,

    HYDRA_GPU_RENDERER_METAL,
} HydraGpuRenderer;

typedef enum : uint32_t {
    HYDRA_SHADER_BACKEND_INVALID = 0,

    HYDRA_SHADER_BACKEND_MSL,
    HYDRA_SHADER_BACKEND_AIR,
} HydraShaderBackend;

typedef enum : uint32_t {
    HYDRA_LOGGING_OUTPUT_INVALID = 0,

    HYDRA_LOGGING_OUTPUT_NONE,
    HYDRA_LOGGING_OUTPUT_STD_OUT,
    HYDRA_LOGGING_OUTPUT_FILE,
} HydraLoggingOutput;

typedef enum : uint32_t {
    HYDRA_LOG_LEVEL_DEBUG,
    HYDRA_LOG_LEVEL_INFO,
    HYDRA_LOG_LEVEL_STUB,
    HYDRA_LOG_LEVEL_WARNING,
    HYDRA_LOG_LEVEL_ERROR,
    HYDRA_LOG_LEVEL_FATAL,
} HydraLogLevel;

typedef enum : uint32_t {
    HYDRA_LOG_CLASS_COMMON,
    HYDRA_LOG_CLASS_MMU,
    HYDRA_LOG_CLASS_CPU,
    HYDRA_LOG_CLASS_GPU,
    HYDRA_LOG_CLASS_ENGINES,
    HYDRA_LOG_CLASS_MACRO,
    HYDRA_LOG_CLASS_SHADER_DECOMPILER,
    HYDRA_LOG_CLASS_METAL_RENDERER,
    HYDRA_LOG_CLASS_SDL3_WINDOW,
    HYDRA_LOG_CLASS_HORIZON,
    HYDRA_LOG_CLASS_KERNEL,
    HYDRA_LOG_CLASS_FILESYSTEM,
    HYDRA_LOG_CLASS_LOADER,
    HYDRA_LOG_CLASS_SERVICES,
    HYDRA_LOG_CLASS_APPLETS,
    HYDRA_LOG_CLASS_CUBEB,
    HYDRA_LOG_CLASS_HYPERVISOR,
    HYDRA_LOG_CLASS_DYNARMIC,
    HYDRA_LOG_CLASS_INPUT,
    HYDRA_LOG_CLASS_OTHER,
} HydraLogClass;

typedef enum : uint32_t {
    HYDRA_DEBUGGER_THREAD_STATUS_RUNNING,
    HYDRA_DEBUGGER_THREAD_STATUS_BREAK,
} HydraDebuggerThreadStatus;

// Options
bool hydra_bool_option_get(const void* option);
void hydra_bool_option_set(void* option, const bool value);

int32_t hydra_i32_option_get(const void* option);
void hydra_i32_option_set(void* option, const int32_t value);

uint32_t hydra_u32_option_get(const void* option);
void hydra_u32_option_set(void* option, const uint32_t value);

__uint128_t hydra_u128_option_get(const void* option);
void hydra_u128_option_set(void* option, const __uint128_t value);

const char* hydra_string_option_get(const void* option);
void hydra_string_option_set(void* option, const char* value);

uint32_t hydra_string_array_option_get_count(const void* option);
const char* hydra_string_array_option_get(const void* option, uint32_t index);
void hydra_string_array_option_add(void* option, const char* value);
void hydra_string_array_option_set(void* option, uint32_t index,
                                   const char* value);
void hydra_string_array_option_remove(void* option, uint32_t index);

// Config
void hydra_config_initialize();
void hydra_config_serialize();
void hydra_config_deserialize();

void* hydra_config_get_game_paths();
void* hydra_config_get_patch_paths();
void* hydra_config_get_sd_card_path();
void* hydra_config_get_save_path();
void* hydra_config_get_cpu_backend();
void* hydra_config_get_gpu_renderer();
void* hydra_config_get_shader_backend();
void* hydra_config_get_user_id();
void* hydra_config_get_logging_output();
void* hydra_config_get_debug_logging();
void* hydra_config_get_stack_trace_logging();
void* hydra_config_get_process_args();

// Emulation context
void* hydra_emulation_context_create();
void hydra_emulation_context_destroy(void* ctx);

void hydra_emulation_context_set_surface(void* ctx, void* surface);
void hydra_emulation_context_load_from_file(void* ctx, const char* filename);
//void hydra_emulation_context_load_from_firmware(void* ctx, TODO);
void hydra_emulation_context_run(void* ctx);

void hydra_emulation_context_progress_frame(void* ctx, uint32_t width,
                                            uint32_t height,
                                            bool* out_dt_average_updated);

uint64_t hydra_emulation_context_get_title_id(void* ctx);

bool hydra_emulation_context_is_running(void* ctx);
float hydra_emulation_context_get_last_delta_time_average(void* ctx);

// Input
// TODO

// Debugger

// Debugger
void hydra_debugger_enable();
void hydra_debugger_disable();
void hydra_debugger_lock();
void hydra_debugger_unlock();
void hydra_debugger_register_this_thread(const char* name);
void hydra_debugger_unregister_this_thread();
size_t hydra_debugger_get_thread_count();
void* hydra_debugger_get_thread(uint32_t index);

// Thread
void hydra_debugger_thread_lock(void* thread);
void hydra_debugger_thread_unlock(void* thread);
const char* hydra_debugger_thread_get_name(void* thread);
HydraDebuggerThreadStatus hydra_debugger_thread_get_status(void* thread);
const char* hydra_debugger_thread_get_break_reason(void* thread);
size_t hydra_debugger_thread_get_message_count(void* thread);
const void* hydra_debugger_thread_get_message(void* thread, uint32_t index);

// Message
HydraLogLevel hydra_debugger_message_get_log_level(const void* msg);
HydraLogClass hydra_debugger_message_get_log_class(const void* msg);
const char* hydra_debugger_message_get_file(const void* msg);
uint32_t hydra_debugger_message_get_line(const void* msg);
const char* hydra_debugger_message_get_function(const void* msg);
const char* hydra_debugger_message_get_string(const void* msg);
const void* hydra_debugger_message_get_stack_trace(const void* msg);

// Stack trace
void* hydra_debugger_stack_trace_copy(const void* stack_trace);
void hydra_debugger_stack_trace_destroy(void* stack_trace);
uint32_t hydra_debugger_stack_trace_get_frame_count(const void* stack_trace);
const void* hydra_debugger_stack_trace_get_frame(const void* stack_trace, uint32_t index);

// Stack frame
void* hydra_debugger_stack_frame_resolve_unmanaged(const void* stack_frame);

// Resolved stack frame
void hydra_debugger_resolved_stack_frame_destroy(void* resolved_stack_frame);
const char* hydra_debugger_resolved_stack_frame_get_module(const void* resolved_stack_frame);
const char* hydra_debugger_resolved_stack_frame_get_function(const void* resolved_stack_frame);
uint64_t hydra_debugger_resolved_stack_frame_get_address(const void* resolved_stack_frame);

#ifdef __cplusplus
}
#endif

#endif
