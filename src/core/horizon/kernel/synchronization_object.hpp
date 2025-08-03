#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

class IThread;

typedef std::function<void()> signal_callback_fn_t;

class SynchronizationObject : public AutoObject {
  public:
    SynchronizationObject(
        bool signalled_ = false,
        const std::string_view debug_name = "SynchronizationObject")
        : AutoObject(debug_name), signalled{signalled_} {}

    void AddWaiter(signal_callback_fn_t waiter);
    void Signal();
    bool Clear();

    // Helpers
    void AddWaitingThread(IThread* thread);

  private:
    std::mutex mutex;
    std::vector<signal_callback_fn_t> waiters;
    bool signalled{false};
};

} // namespace hydra::horizon::kernel
