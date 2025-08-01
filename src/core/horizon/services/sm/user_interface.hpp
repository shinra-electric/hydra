#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::sm {

class IUserInterface : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(RegisterClient);
    result_t GetServiceHandle(kernel::Process* process, u64 name,
                              OutHandle<HandleAttr::Move> out_handle);
    result_t RegisterService(kernel::Process* process, u64 name, bool is_light,
                             i32 max_sessions,
                             OutHandle<HandleAttr::Move> out_port_handle);
};

} // namespace hydra::horizon::services::sm
