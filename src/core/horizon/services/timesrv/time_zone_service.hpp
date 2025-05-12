#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

class ITimeZoneService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ToCalendarTimeWithMyRule);
};

} // namespace Hydra::Horizon::Services::TimeSrv
