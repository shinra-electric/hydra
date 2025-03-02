#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

class IStaticService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStaticService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    void GetStandardUserSystemClock(REQUEST_COMMAND_PARAMS);
    void GetStandardNetworkSystemClock(REQUEST_COMMAND_PARAMS);
    void GetStandardSteadyClock(REQUEST_COMMAND_PARAMS);
    void GetTimeZoneService(REQUEST_COMMAND_PARAMS);
    void GetStandardLocalSystemClock(REQUEST_COMMAND_PARAMS);
    void GetEphemeralNetworkSystemClock(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::TimeSrv
