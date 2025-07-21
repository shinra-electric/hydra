#include "core/horizon/kernel/synchronization_object.hpp"

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel {

bool SynchronizationObject::AddWaitingThread(Thread* thread) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!signalled) {
        waiting_threads.push_back(thread);
        thread->Pause();
    }

    return !signalled;
}

void SynchronizationObject::Signal() {
    std::lock_guard<std::mutex> lock(mutex);
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
