#pragma once

#include "core/horizon/kernel/const.hpp"
#include "core/horizon/kernel/synchronization_object.hpp"
#include "core/hw/tegra_x1/cpu/memory.hpp"

namespace hydra::horizon::kernel {

class Process;

enum class ThreadState {
    Created,
    Running,
    Stopping,
    Stopped,
    Paused,
};

enum class ThreadMessageType {
    Stop,
    Pause,
    Resume,
};

struct ThreadMessage {
    ThreadMessageType type;
    union {
        // Resume
        struct {
            SynchronizationObject* signalled_obj;
        } resume;
    } payload;
};

enum class ThreadActionType {
    None,
    Stop,
    Resume,
};

struct ThreadAction {
    ThreadActionType type{ThreadActionType::None};
    union {
        // Resume
        struct {
            SynchronizationObject* signalled_obj;
        } resume;
    } payload;
};

class Thread : public SynchronizationObject {
  public:
    Thread(Process* process_, vaddr_t stack_top_addr_, i32 priority_,
           const std::string_view debug_name = "Thread");
    ~Thread() override;

    void Start();

    void SetEntryPoint(vaddr_t entry_point_) { entry_point = entry_point_; }
    void SetArg(u32 index, u64 value) {
        ASSERT(index < sizeof_array(args), Kernel, "Invalid argument index {}",
               index);
        args[index] = value;
    }

    // Messages
    void Stop() { SendMessage({.type = ThreadMessageType::Stop}); }
    void Pause() { SendMessage({.type = ThreadMessageType::Pause}); }
    void Resume(SynchronizationObject* signalled_obj = nullptr) {
        SendMessage({.type = ThreadMessageType::Resume,
                     .payload = {.resume = {.signalled_obj = signalled_obj}}});
    }

    // Must not be called from a different thread
    ThreadAction ProcessMessages(i64 pause_timeout_ns = INFINITE_TIMEOUT);

  private:
    Process* process;

    hw::tegra_x1::cpu::IMemory* tls_mem;
    vaddr_t tls_addr;
    vaddr_t stack_top_addr;
    i32 priority;

    vaddr_t entry_point{0};
    u64 args[2] = {0};

    std::thread* thread{nullptr};

    ThreadState state{ThreadState::Created}; // TODO: atomic?

    std::mutex msg_mutex;
    std::condition_variable msg_cv;
    std::queue<ThreadMessage> msg_queue;

    // Helpers
    void SendMessage(ThreadMessage msg);
    ThreadAction ProcessMessagesImpl();

  public:
    GETTER(state, GetState);
};

} // namespace hydra::horizon::kernel
