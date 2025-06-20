#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IAllSystemAppletProxiesService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    // TODO: params
    result_t OpenLibraryAppletProxyOld(add_service_fn_t add_service);
    // TODO: PID and type-0x15 buffer nn::am::AppletAttribute
    result_t
    OpenLibraryAppletProxy(add_service_fn_t add_service, u64 _reserved_x0,
                           InHandle<HandleAttr::Copy> crnt_process_handle);

    // Impl
    result_t OpenLibraryAppletProxyImpl(add_service_fn_t add_service);
};

} // namespace hydra::horizon::services::am
