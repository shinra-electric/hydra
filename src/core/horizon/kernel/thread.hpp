#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {

class Process;

constexpr u64 TLS_SIZE = 0x20000; // TODO: what should this be?

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

enum class ThreadResumeReason {
    Signalled,
    TimedOut,
    Cancelled,
};

struct ThreadAction {
    ThreadActionType type{ThreadActionType::None};
    union {
        // Resume
        struct {
            ThreadResumeReason reason;
            SynchronizationObject* signalled_obj;
        } resume;
    } payload;
};

class IThread : public SynchronizationObject {
  public:
    IThread(Process* process_, i32 priority_,
            const std::string_view debug_name = "Thread")
        : SynchronizationObject(false, debug_name), process{process_},
          priority{priority_} {}
    virtual ~IThread() override;

    void Start();

    // Messages
    void Stop() { SendMessage({.type = ThreadMessageType::Stop}); }
    void Pause() { SendMessage({.type = ThreadMessageType::Pause}); }
    void Resume(SynchronizationObject* signalled_obj = nullptr) {
        SendMessage({.type = ThreadMessageType::Resume,
                     .payload = {.resume = {.signalled_obj = signalled_obj}}});
    }

    // Must not be called from a different thread
    ThreadAction ProcessMessages(i64 pause_timeout_ns = INFINITE_TIMEOUT);

    virtual uptr GetTlsPtr() const = 0;

  protected:
    Process* process;

    virtual void Run() = 0;

  private:
    i32 priority;

    std::thread* thread{nullptr};

    ThreadState state{ThreadState::Created}; // TODO: atomic?

    std::mutex msg_mutex;
    std::condition_variable msg_cv;
    std::queue<ThreadMessage> msg_queue;

    // Helpers
    void SendMessage(ThreadMessage msg);
    ThreadAction ProcessMessagesImpl();

  public:
    GETTER(process, GetProcess);
    GETTER(state, GetState);
};

inline thread_local IThread* tls_current_thread = nullptr;

} // namespace hydra::horizon::kernel
