#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IAllSystemAppletProxiesService : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    // TODO: correct?
    result_t OpenSystemAppletProxy(RequestContext* ctx);
    // TODO: correct?
    result_t OpenLibraryAppletProxyOld(RequestContext* ctx);
    // TODO: PID and type-0x15 buffer nn::am::AppletAttribute
    result_t
    OpenLibraryAppletProxy(RequestContext* ctx, u64 _reserved_x0,
                           InHandle<HandleAttr::Copy> crnt_process_handle);
    // TODO: correct?
    result_t OpenOverlayAppletProxy(RequestContext* ctx);
    // TODO: correct?
    result_t OpenSystemApplicationProxy(RequestContext* ctx);

    // Impl
    result_t OpenLibraryAppletProxyImpl(RequestContext* ctx);
};

} // namespace hydra::horizon::services::am
