#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

enum class PerformanceMode : i32 {
    Invalid = -1,
    Normal = 0,
    Boost = 1,

    Count,
};

class ISession : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    u32 performance_configs[(u32)PerformanceMode::Count] = {
        0x20004, 0x92220007}; // TODO: what should this be?

    // Commands
    result_t SetPerformanceConfiguration(PerformanceMode mode, u32 config);
    result_t GetPerformanceConfiguration(PerformanceMode mode, u32* out_config);
};

} // namespace hydra::horizon::services::am

ENABLE_ENUM_FORMATTING(hydra::horizon::services::am::PerformanceMode, Invalid,
                       "invalid", Normal, "normal", Boost, "boost");
