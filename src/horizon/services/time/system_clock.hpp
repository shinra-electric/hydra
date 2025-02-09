#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Time {

enum class SystemClockType {
    StandardUser,
    StandardNetwork,
    StandardLocal,
    EphemeralNetwork,
};

class SystemClock : public ServiceBase {
  public:
    SystemClock(Handle handle_, SystemClockType type_)
        : ServiceBase(handle_), type{type_} {}

    void Request(Kernel& kernel, Writer& writer, Writer& move_handles_writer,
                 u8* in_ptr) override;

  private:
    SystemClockType type;
};

} // namespace Hydra::Horizon::Services::Time
