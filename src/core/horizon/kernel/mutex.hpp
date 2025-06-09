#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

// TODO: should this be used?
constexpr u32 HANDLE_WAIT_MASK = 0x40000000;

class Mutex {
  public:
    void Lock(u32& value, u32 self_tag) {
        std::unique_lock<std::mutex> lock(mutex);
        // TODO: why is this necessary?
        value = value | HANDLE_WAIT_MASK;
        cv.wait(lock, [&] { return (value & ~HANDLE_WAIT_MASK) == 0; });
        value = self_tag | (value & HANDLE_WAIT_MASK);
    }

    void Unlock(u32& value) {
        std::unique_lock<std::mutex> lock(mutex);
        value = (value & HANDLE_WAIT_MASK);
        cv.notify_one();
    }

    // Getters
    std::mutex& GetNativeHandle() { return mutex; }

  private:
    std::mutex mutex;
    std::condition_variable cv;
};

} // namespace hydra::horizon::kernel
