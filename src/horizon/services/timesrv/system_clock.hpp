#pragma once

#include "horizon/services/service.hpp"

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

    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
    SystemClockType type;
};

} // namespace Hydra::Horizon::Services::TimeSrv
