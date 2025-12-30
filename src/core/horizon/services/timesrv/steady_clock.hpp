#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::timesrv {

#pragma pack(push, 1)
struct SteadyClockTimePoint {
    u64 time_point;
    uuid_t clock_source_id;
};
#pragma pack(pop)

class ISteadyClock : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetCurrentTimePoint(SteadyClockTimePoint* out_time_point);
};

} // namespace hydra::horizon::services::timesrv
