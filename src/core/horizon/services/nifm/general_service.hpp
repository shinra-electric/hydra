#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::nifm {

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

} // namespace hydra::horizon::services::nifm
