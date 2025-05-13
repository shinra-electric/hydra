#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::timesrv {

struct LocationName {
    char name[0x24];
};

struct TimeZoneRule {
    u8 data[0x4000];
};

class ITimeZoneService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetDeviceLocationName(LocationName* out_name);
    result_t LoadTimeZoneRule(LocationName location_name,
                              OutBuffer<BufferAttr::MapAlias> out_rule_buffer);
    STUB_REQUEST_COMMAND(ToCalendarTimeWithMyRule);
};

} // namespace hydra::horizon::services::timesrv
