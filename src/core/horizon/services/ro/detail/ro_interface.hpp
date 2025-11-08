#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::ro::detail {

class IRoInterface : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    // TODO: PID
    result_t MapManualLoadModuleMemory(kernel::Process* process,
                                       u64 pid_reserved, u64 nro_addr,
                                       u64 nro_size, u64 bss_addr, u64 bss_size,
                                       u64* out_addr);
    STUB_REQUEST_COMMAND(RegisterModuleInfo);
    STUB_REQUEST_COMMAND(RegisterProcessHandle);
};

} // namespace hydra::horizon::services::ro::detail
