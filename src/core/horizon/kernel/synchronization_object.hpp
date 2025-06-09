#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

// TODO: this shouldn't handle signaling and waiting, that's a job for the
class SynchronizationObject : public AutoObject {
  public:
    SynchronizationObject(bool signaled_ = false) : signaled{signaled_} {}

    void Signal() {
        std::unique_lock<std::mutex> lock(mutex);
        signaled = true;
        cv.notify_all();
    }

    bool Clear() {
        bool was_signaled;
        {
            std::unique_lock<std::mutex> lock(mutex);
            was_signaled = signaled;
            signaled = false;
        }

        return was_signaled;
    }

    virtual bool Wait(i64 timeout = INFINITE_TIMEOUT) {
        std::unique_lock<std::mutex> lock(mutex);
        return WaitImpl(lock, timeout);
    }

  protected:
    std::mutex mutex;
    bool signaled; // TODO: atomic? (probably not necessary though)

    // Returns true if the event was signaled, false on timeout
    bool WaitImpl(std::unique_lock<std::mutex>& lock, i64 timeout) {
        // First, check if the event is already signaled
        if (signaled)
            return true;
        else if (timeout == 0)
            return false;

        if (timeout == INFINITE_TIMEOUT) {
            cv.wait(lock);
            return true;
        } else {
            const auto status =
                cv.wait_for(lock, std::chrono::nanoseconds(timeout));
            return (status == std::cv_status::no_timeout);
        }
    }

  private:
    std::condition_variable cv;
};

} // namespace hydra::horizon::kernel
