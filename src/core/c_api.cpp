#include "core/c_api.h"

#include "core/debugger/debugger.hpp"
#include "core/emulation_context.hpp"
#include "core/horizon/ui/handler_base.hpp"

#define HYDRA_EXPORT extern "C" __attribute__((visibility("default")))

// Options
HYDRA_EXPORT bool hydra_bool_option_get(const void* option) {
    return static_cast<const hydra::Option<bool>*>(option)->Get();
}

HYDRA_EXPORT void hydra_bool_option_set(void* option, const bool value) {
    static_cast<hydra::Option<bool>*>(option)->Set(value);
}

HYDRA_EXPORT int32_t hydra_i32_option_get(const void* option) {
    return static_cast<const hydra::Option<hydra::i32>*>(option)->Get();
}

HYDRA_EXPORT void hydra_i32_option_set(void* option, const int32_t value) {
    static_cast<hydra::Option<hydra::i32>*>(option)->Set(value);
}

HYDRA_EXPORT uint32_t hydra_u32_option_get(const void* option) {
    return static_cast<const hydra::Option<hydra::i32>*>(option)->Get();
}

HYDRA_EXPORT void hydra_u32_option_set(void* option, const uint32_t value) {
    static_cast<hydra::Option<hydra::i32>*>(option)->Set(value);
}

HYDRA_EXPORT __uint128_t hydra_u128_option_get(const void* option) {
    return static_cast<const hydra::Option<hydra::u128>*>(option)->Get();
}

HYDRA_EXPORT void hydra_u128_option_set(void* option, const __uint128_t value) {
    static_cast<hydra::Option<hydra::u128>*>(option)->Set(value);
}

HYDRA_EXPORT const char* hydra_string_option_get(const void* option) {
    return static_cast<const hydra::Option<std::string>*>(option)
        ->Get()
        .c_str();
}

HYDRA_EXPORT void hydra_string_option_set(void* option, const char* value) {
    static_cast<hydra::Option<std::string>*>(option)->Set(value);
}

HYDRA_EXPORT uint32_t hydra_string_array_option_get_count(const void* option) {
    return static_cast<const hydra::ArrayOption<std::string>*>(option)
        ->GetCount();
}

HYDRA_EXPORT const char* hydra_string_array_option_get(const void* option,
                                                       uint32_t index) {
    return static_cast<const hydra::ArrayOption<std::string>*>(option)
        ->Get(index)
        .c_str();
}

HYDRA_EXPORT void hydra_string_array_option_add(void* option,
                                                const char* value) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Add(value);
}

HYDRA_EXPORT void hydra_string_array_option_set(void* option, uint32_t index,
                                                const char* value) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Set(index, value);
}

HYDRA_EXPORT void hydra_string_array_option_remove(void* option,
                                                   uint32_t index) {
    static_cast<hydra::ArrayOption<std::string>*>(option)->Remove(index);
}

// Config
HYDRA_EXPORT void hydra_config_initialize() {
    hydra::CONFIG_INSTANCE.Initialize();
}

HYDRA_EXPORT void hydra_config_serialize() {
    hydra::CONFIG_INSTANCE.Serialize();
}

HYDRA_EXPORT void hydra_config_deserialize() {
    hydra::CONFIG_INSTANCE.Deserialize();
}

HYDRA_EXPORT void* hydra_config_get_game_paths() {
    return &hydra::CONFIG_INSTANCE.GetGamePaths();
}

HYDRA_EXPORT void* hydra_config_get_patch_paths() {
    return &hydra::CONFIG_INSTANCE.GetPatchPaths();
}

HYDRA_EXPORT void* hydra_config_get_sd_card_path() {
    return &hydra::CONFIG_INSTANCE.GetSdCardPath();
}

HYDRA_EXPORT void* hydra_config_get_save_path() {
    return &hydra::CONFIG_INSTANCE.GetSavePath();
}

HYDRA_EXPORT void* hydra_config_get_cpu_backend() {
    return &hydra::CONFIG_INSTANCE.GetCpuBackend();
}

HYDRA_EXPORT void* hydra_config_get_gpu_renderer() {
    return &hydra::CONFIG_INSTANCE.GetGpuRenderer();
}

HYDRA_EXPORT void* hydra_config_get_shader_backend() {
    return &hydra::CONFIG_INSTANCE.GetShaderBackend();
}

HYDRA_EXPORT void* hydra_config_get_user_id() {
    return &hydra::CONFIG_INSTANCE.GetUserID();
}

HYDRA_EXPORT void* hydra_config_get_logging_output() {
    return &hydra::CONFIG_INSTANCE.GetLoggingOutput();
}

HYDRA_EXPORT void* hydra_config_get_debug_logging() {
    return &hydra::CONFIG_INSTANCE.GetDebugLogging();
}

HYDRA_EXPORT void* hydra_config_get_stack_trace_logging() {
    return &hydra::CONFIG_INSTANCE.GetStackTraceLogging();
}

HYDRA_EXPORT void* hydra_config_get_process_args() {
    return &hydra::CONFIG_INSTANCE.GetProcessArgs();
}

// Emulation context
// TODO: proper UI handler
class UIHandler : public hydra::horizon::ui::HandlerBase {
  public:
    void ShowMessageDialog(const hydra::horizon::ui::MessageDialogType type,
                           const std::string& title,
                           const std::string& message) override {}
    hydra::horizon::applets::SoftwareKeyboardResult
    ShowSoftwareKeyboard(const std::string& header_text,
                         std::string& out_text) override {
        return hydra::horizon::applets::SoftwareKeyboardResult::OK;
    }
};

HYDRA_EXPORT void* hydra_emulation_context_create() {
    return new hydra::EmulationContext(*(new UIHandler()));
}

HYDRA_EXPORT void hydra_emulation_context_destroy(void* ctx) {
    // TODO: also destroy the UI handler
    delete static_cast<hydra::EmulationContext*>(ctx);
}

HYDRA_EXPORT void hydra_emulation_context_set_surface(void* ctx,
                                                      void* surface) {
    static_cast<hydra::EmulationContext*>(ctx)->SetSurface(surface);
}

HYDRA_EXPORT void hydra_emulation_context_load_rom(void* ctx,
                                                   const char* rom_filename) {
    static_cast<hydra::EmulationContext*>(ctx)->LoadRom(rom_filename);
}

HYDRA_EXPORT void hydra_emulation_context_run(void* ctx) {
    static_cast<hydra::EmulationContext*>(ctx)->Run();
}

HYDRA_EXPORT void hydra_emulation_context_progress_frame(
    void* ctx, uint32_t width, uint32_t height, bool* out_dt_average_updated) {
    static_cast<hydra::EmulationContext*>(ctx)->ProgressFrame(
        width, height, *out_dt_average_updated);
}

HYDRA_EXPORT uint64_t hydra_emulation_context_get_title_id(void* ctx) {
    return static_cast<hydra::EmulationContext*>(ctx)->GetTitleID();
}

HYDRA_EXPORT bool hydra_emulation_context_is_running(void* ctx) {
    return static_cast<hydra::EmulationContext*>(ctx)->IsRunning();
}

HYDRA_EXPORT float
hydra_emulation_context_get_last_delta_time_average(void* ctx) {
    return static_cast<hydra::EmulationContext*>(ctx)
        ->GetLastDeltaTimeAverage();
}

// Debugger

// Debugger
HYDRA_EXPORT void hydra_debugger_enable() { hydra::DEBUGGER_INSTANCE.Enable(); }

HYDRA_EXPORT void hydra_debugger_disable() {
    hydra::DEBUGGER_INSTANCE.Disable();
}

HYDRA_EXPORT void hydra_debugger_lock() { hydra::DEBUGGER_INSTANCE.Lock(); }

HYDRA_EXPORT void hydra_debugger_unlock() { hydra::DEBUGGER_INSTANCE.Unlock(); }

HYDRA_EXPORT void hydra_debugger_register_this_thread(const char* name) {
    hydra::DEBUGGER_INSTANCE.RegisterThisThread(name);
}

HYDRA_EXPORT void hydra_debugger_unregister_this_thread() {
    hydra::DEBUGGER_INSTANCE.UnregisterThisThread();
}

HYDRA_EXPORT size_t hydra_debugger_get_thread_count() {
    return hydra::DEBUGGER_INSTANCE.GetThreadCount();
}

HYDRA_EXPORT void* hydra_debugger_get_thread(uint32_t index) {
    return &hydra::DEBUGGER_INSTANCE.GetThread(index);
}

// Thread
HYDRA_EXPORT void hydra_debugger_thread_lock(void* thread) {
    static_cast<hydra::debugger::Thread*>(thread)->Lock();
}

HYDRA_EXPORT void hydra_debugger_thread_unlock(void* thread) {
    static_cast<hydra::debugger::Thread*>(thread)->Unlock();
}

HYDRA_EXPORT const char* hydra_debugger_thread_get_name(void* thread) {
    return static_cast<hydra::debugger::Thread*>(thread)->GetName().c_str();
}

HYDRA_EXPORT size_t hydra_debugger_thread_get_message_count(void* thread) {
    return static_cast<hydra::debugger::Thread*>(thread)->GetMessageCount();
}

HYDRA_EXPORT const void* hydra_debugger_thread_get_message(void* thread,
                                                           uint32_t index) {
    return &static_cast<hydra::debugger::Thread*>(thread)->GetMessage(index);
}

// Message
HYDRA_EXPORT HydraLogLevel
hydra_debugger_message_get_log_level(const void* msg) {
    return static_cast<HydraLogLevel>(
        static_cast<const hydra::debugger::Message*>(msg)->log.level);
}

HYDRA_EXPORT HydraLogClass
hydra_debugger_message_get_log_class(const void* msg) {
    return static_cast<HydraLogClass>(
        static_cast<const hydra::debugger::Message*>(msg)->log.c);
}

HYDRA_EXPORT const char* hydra_debugger_message_get_file(const void* msg) {
    return static_cast<const hydra::debugger::Message*>(msg)->log.file.c_str();
}

HYDRA_EXPORT uint32_t hydra_debugger_message_get_line(const void* msg) {
    return static_cast<const hydra::debugger::Message*>(msg)->log.line;
}

HYDRA_EXPORT const char* hydra_debugger_message_get_function(const void* msg) {
    return static_cast<const hydra::debugger::Message*>(msg)
        ->log.function.c_str();
}

HYDRA_EXPORT const char* hydra_debugger_message_get_string(const void* msg) {
    return static_cast<const hydra::debugger::Message*>(msg)->log.str.c_str();
}

HYDRA_EXPORT const void*
hydra_debugger_message_get_stack_trace(const void* msg) {
    return &static_cast<const hydra::debugger::Message*>(msg)->stack_trace;
}

// Stack trace
HYDRA_EXPORT void* hydra_debugger_stack_trace_copy(const void* stack_trace) {
    return new hydra::debugger::StackTrace(*static_cast<const hydra::debugger::StackTrace*>(stack_trace));
}

HYDRA_EXPORT void hydra_debugger_stack_trace_destroy(void* stack_trace) {
    delete static_cast<hydra::debugger::StackTrace*>(stack_trace);
}

HYDRA_EXPORT uint32_t
hydra_debugger_stack_trace_get_frame_count(const void* stack_trace) {
    return static_cast<const hydra::debugger::StackTrace*>(stack_trace)
        ->frames.size();
}

HYDRA_EXPORT const void*
hydra_debugger_stack_trace_get_frame(const void* stack_trace, uint32_t index) {
    return &static_cast<const hydra::debugger::StackTrace*>(stack_trace)
                ->frames[index];
}

// Stack frame
HYDRA_EXPORT void* hydra_debugger_stack_frame_resolve_unmanaged(const void* stack_frame) {
    return new hydra::debugger::ResolvedStackFrame(
        static_cast<const hydra::debugger::StackFrame*>(stack_frame)
            ->Resolve());
}

// Resolved stack frame
HYDRA_EXPORT void
hydra_debugger_resolved_stack_frame_destroy(void* resolved_stack_frame) {
    delete static_cast<hydra::debugger::ResolvedStackFrame*>(
        resolved_stack_frame);
}

HYDRA_EXPORT const char* hydra_debugger_resolved_stack_frame_get_module(
    const void* resolved_stack_frame) {
    return static_cast<const hydra::debugger::ResolvedStackFrame*>(
               resolved_stack_frame)
        ->module.c_str();
}

HYDRA_EXPORT const char* hydra_debugger_resolved_stack_frame_get_function(
    const void* resolved_stack_frame) {
    return static_cast<const hydra::debugger::ResolvedStackFrame*>(
               resolved_stack_frame)
        ->function.c_str();
}

HYDRA_EXPORT uint64_t hydra_debugger_resolved_stack_frame_get_address(
    const void* resolved_stack_frame) {
    return static_cast<const hydra::debugger::ResolvedStackFrame*>(
               resolved_stack_frame)
        ->addr;
}
