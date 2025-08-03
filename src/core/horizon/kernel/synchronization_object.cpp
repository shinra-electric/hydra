#include "core/horizon/kernel/synchronization_object.hpp"

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel {

void SynchronizationObject::AddWaiter(signal_callback_fn_t waiter) {
    std::lock_guard lock(mutex);
    if (signalled)
        waiter();
    else
        waiters.push_back(waiter);
}

void SynchronizationObject::Signal() {
    std::lock_guard lock(mutex);
    if (signalled)
        return;

    signalled = true;

    for (auto waiter : waiters)
        waiter();
    waiters.clear();
}

bool SynchronizationObject::Clear() {
    bool was_signalled;
    {
        std::unique_lock<std::mutex> lock(mutex);
        was_signalled = signalled;
        signalled = false;
    }

    return was_signalled;
}

void SynchronizationObject::AddWaitingThread(IThread* thread) {
    AddWaiter([this, thread]() { thread->Resume(this); });
}

} // namespace hydra::horizon::kernel
