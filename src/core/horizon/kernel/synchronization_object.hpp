#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

class IThread;

class SynchronizationObject : public AutoObject {
  public:
    SynchronizationObject(
        bool signalled_ = false,
        const std::string_view debug_name = "SynchronizationObject")
        : AutoObject(debug_name), signalled{signalled_} {}

    void AddWaitingThread(IThread* thread);
    void Signal();
    bool Clear();

  private:
    std::mutex mutex;
    std::vector<IThread*> waiting_threads;
    bool signalled{false};
};

} // namespace hydra::horizon::kernel
