#include "core/horizon/kernel/synchronization_object.hpp"

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel {

void SynchronizationObject::AddWaitingThread(IThread* thread) {
    std::lock_guard lock(mutex);
    if (signalled)
        thread->Resume(this);
    else
        waiting_threads.push_back(thread);
}

void SynchronizationObject::Signal() {
    std::lock_guard lock(mutex);
    signalled = true;
    for (auto thread : waiting_threads)
        thread->Resume(this);
    waiting_threads.clear();
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

} // namespace hydra::horizon::kernel
