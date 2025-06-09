#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {

// TODO: ReadableEvent and WritableEvent
class Event : public SynchronizationObject {
  public:
    Event(bool autoclear_ = false, bool signaled = false)
        : SynchronizationObject(signaled), autoclear{autoclear_} {}

    bool Wait(i64 timeout = INFINITE_TIMEOUT) override {
        std::unique_lock<std::mutex> lock(mutex);
        bool was_signaled = WaitImpl(lock, timeout);
        // TODO: correct?
        if (autoclear)
            signaled = false;

        return was_signaled;
    }

  private:
    bool autoclear;
};

} // namespace hydra::horizon::kernel
