#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Am {

enum class PerformanceMode : i32 {
    Invalid = -1,
    Normal = 0,
    Boost = 1,
};

class ISession : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t SetPerformanceConfiguration(PerformanceMode mode, u32 config);
};

} // namespace Hydra::Horizon::Services::Am

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::Am::PerformanceMode, Invalid,
                       "invalid", Normal, "normal", Boost, "boost");
