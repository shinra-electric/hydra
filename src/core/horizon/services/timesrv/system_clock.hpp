#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

enum class SystemClockType {
    StandardUser,
    StandardNetwork,
    StandardLocal,
    EphemeralNetwork,
};

class ISystemClock : public ServiceBase {
  public:
    ISystemClock(SystemClockType type_) : type{type_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    SystemClockType type;

    // Commands
    STUB_REQUEST_COMMAND(GetCurrentTime);
};

} // namespace Hydra::Horizon::Services::TimeSrv
