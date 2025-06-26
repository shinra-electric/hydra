#include "core/horizon/services/am/all_system_applet_proxies_service.hpp"

#include "core/horizon/services/am/library_applet_proxy.hpp"
#include "core/horizon/services/am/system_applet_proxy.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IAllSystemAppletProxiesService, 100,
                             OpenSystemAppletProxy, 200,
                             OpenLibraryAppletProxyOld, 201,
                             OpenLibraryAppletProxy)

result_t IAllSystemAppletProxiesService::OpenSystemAppletProxy(
    add_service_fn_t add_service) {
    add_service(new ISystemAppletProxy());
    return RESULT_SUCCESS;
}

result_t IAllSystemAppletProxiesService::OpenLibraryAppletProxyOld(
    add_service_fn_t add_service) {
    return OpenLibraryAppletProxyImpl(add_service);
}

result_t IAllSystemAppletProxiesService::OpenLibraryAppletProxy(
    add_service_fn_t add_service, u64 _reserved_x0,
    InHandle<HandleAttr::Copy> crnt_process_handle) {
    return OpenLibraryAppletProxyImpl(add_service);
}

result_t IAllSystemAppletProxiesService::OpenLibraryAppletProxyImpl(
    add_service_fn_t add_service) {
    add_service(new ILibraryAppletProxy());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
