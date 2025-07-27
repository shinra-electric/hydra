#pragma once

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel {

typedef std::function<bool()> should_stop_fn_t;
typedef std::function<void(should_stop_fn_t)> run_callback_fn_t;

class HostThread : public IThread {
  public:
    HostThread(Process* process, i32 priority, run_callback_fn_t run_callback_,
               const std::string_view debug_name = "Thread")
        : IThread(process, priority, debug_name), run_callback{run_callback_} {}
    ~HostThread() override { delete[] tls; }

    uptr GetTlsPtr() const override { return reinterpret_cast<uptr>(tls); }

  protected:
    void Run() override {
        run_callback([this]() {
            ProcessMessages();
            return GetState() == ThreadState::Stopping;
        });
    }

  private:
    run_callback_fn_t run_callback;

    u8* tls = new u8[TLS_SIZE]; // TODO: stack allocate
};

} // namespace hydra::horizon::kernel
