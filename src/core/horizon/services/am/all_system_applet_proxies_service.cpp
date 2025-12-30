#include "core/horizon/services/am/all_system_applet_proxies_service.hpp"

#include "core/horizon/services/am/application_proxy.hpp"
#include "core/horizon/services/am/library_applet_proxy.hpp"
#include "core/horizon/services/am/overlay_applet_proxy.hpp"
#include "core/horizon/services/am/system_applet_proxy.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IAllSystemAppletProxiesService, 100,
                             OpenSystemAppletProxy, 200,
                             OpenLibraryAppletProxyOld, 201,
                             OpenLibraryAppletProxy, 300,
                             OpenOverlayAppletProxy, 350,
                             OpenSystemApplicationProxy)

result_t
IAllSystemAppletProxiesService::OpenSystemAppletProxy(RequestContext* ctx) {
    AddService(*ctx, new ISystemAppletProxy());
    return RESULT_SUCCESS;
}

result_t
IAllSystemAppletProxiesService::OpenLibraryAppletProxyOld(RequestContext* ctx) {
    return OpenLibraryAppletProxyImpl(ctx);
}

result_t IAllSystemAppletProxiesService::OpenLibraryAppletProxy(
    RequestContext* ctx, [[maybe_unused]] u64 _reserved_x0,
    InHandle<HandleAttr::Copy> crnt_process_handle) {
    (void)crnt_process_handle;
    return OpenLibraryAppletProxyImpl(ctx);
}

result_t IAllSystemAppletProxiesService::OpenLibraryAppletProxyImpl(
    RequestContext* ctx) {
    AddService(*ctx, new ILibraryAppletProxy());
    return RESULT_SUCCESS;
}

result_t
IAllSystemAppletProxiesService::OpenOverlayAppletProxy(RequestContext* ctx) {
    AddService(*ctx, new IOverlayAppletProxy());
    return RESULT_SUCCESS;
}

result_t IAllSystemAppletProxiesService::OpenSystemApplicationProxy(
    RequestContext* ctx) {
    AddService(*ctx, new IApplicationProxy());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
