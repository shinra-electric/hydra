#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::ns {

class IApplicationManagerInterface : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
};

} // namespace hydra::horizon::services::ns
