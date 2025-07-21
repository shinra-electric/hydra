#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

class Thread;

class SynchronizationObject : public AutoObject {
  public:
    SynchronizationObject(
        bool signalled_ = false,
        const std::string_view debug_name = "SynchronizationObject")
        : AutoObject(debug_name), signalled{signalled_} {}

    bool AddWaitingThread(Thread* thread);
    void Signal();
    bool Clear();

  private:
    std::mutex mutex;
    std::vector<Thread*> waiting_threads;
    bool signalled{false};
};

} // namespace hydra::horizon::kernel
