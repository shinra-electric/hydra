#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {

enum class EventFlags {
    None = 0,
    AutoClear = BIT(0),
    Signalled = BIT(1),
};
ENABLE_ENUM_BITMASK_OPERATORS(EventFlags)

// TODO: ReadableEvent and WritableEvent
class Event : public SynchronizationObject {
  public:
    Event(const EventFlags flags = EventFlags::None,
          const std::string_view debug_name = "Event")
        : SynchronizationObject(any(flags & EventFlags::Signalled), debug_name),
          autoclear{any(flags & EventFlags::AutoClear)} {}

    bool Wait(i64 timeout = INFINITE_TIMEOUT) override {
        std::unique_lock<std::mutex> lock(mutex);
        bool was_signalled = WaitImpl(lock, timeout);
        // TODO: correct?
        if (autoclear)
            signalled = false;

        return was_signalled;
    }

  private:
    bool autoclear;
};

} // namespace hydra::horizon::kernel
