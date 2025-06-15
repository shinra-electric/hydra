#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

// TODO: this shouldn't handle signaling and waiting, that's a job for the
class SynchronizationObject : public AutoObject {
  public:
    SynchronizationObject(
        bool signalled_ = false,
        const std::string_view debug_name = "SynchronizationObject")
        : AutoObject(debug_name), signalled{signalled_} {}

    void Signal() {
        std::unique_lock<std::mutex> lock(mutex);
        signalled = true;
        cv.notify_all();
    }

    bool Clear() {
        bool was_signalled;
        {
            std::unique_lock<std::mutex> lock(mutex);
            was_signalled = signalled;
            signalled = false;
        }

        return was_signalled;
    }

    virtual bool Wait(i64 timeout = INFINITE_TIMEOUT) {
        std::unique_lock<std::mutex> lock(mutex);
        return WaitImpl(lock, timeout);
    }

  protected:
    std::mutex mutex;
    bool signalled; // TODO: atomic? (probably not necessary though)

    // Returns true if the event was signalled, false on timeout
    bool WaitImpl(std::unique_lock<std::mutex>& lock, i64 timeout) {
        // First, check if the event is already signalled
        if (signalled)
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
