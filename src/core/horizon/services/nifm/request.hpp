#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::nifm {

enum class RequestState {
    Invalid,
    Free,
    OnHold,
    Accepted,
    Blocking,
};

class IRequest : public ServiceBase {
  public:
    IRequest();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> events[2];

    // Commands
    result_t GetRequestState(RequestState* out_state);
    STUB_REQUEST_COMMAND(GetResult);
    result_t
    GetSystemEventReadableHandles(OutHandle<HandleAttr::Copy> out_handle0,
                                  OutHandle<HandleAttr::Copy> out_handle1);
};

} // namespace hydra::horizon::services::nifm
