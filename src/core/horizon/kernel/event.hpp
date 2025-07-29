#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {

// TODO: ReadableEvent and WritableEvent
class Event : public SynchronizationObject {
    using SynchronizationObject::SynchronizationObject;
};

} // namespace hydra::horizon::kernel
