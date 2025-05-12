#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::timesrv {

class ITimeZoneService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ToCalendarTimeWithMyRule);
};

} // namespace hydra::horizon::services::timesrv
