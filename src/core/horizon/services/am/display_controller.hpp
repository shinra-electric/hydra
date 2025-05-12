#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Am {

class IDisplayController : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Am
