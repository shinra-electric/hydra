#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Nifm {

class IGeneralService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateRequest(add_service_fn_t add_service,
                           i32 requirement_preset);
    result_t
    GetCurrentNetworkProfile(OutBuffer<BufferAttr::HipcPointer> out_buffer);
};

} // namespace Hydra::Horizon::Services::Nifm
