#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

class ITimeZoneService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ITimeZoneService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ToCalendarTimeWithMyRule);
};

} // namespace Hydra::Horizon::Services::TimeSrv
