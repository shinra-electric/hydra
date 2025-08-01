#include "core/horizon/services/am/application_proxy.hpp"

#include "core/horizon/services/am/application_functions.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 11,
                             GetLibraryAppletCreator, 20,
                             GetApplicationFunctions, 1000, GetDebugFunctions)

result_t IApplicationProxy::GetApplicationFunctions(RequestContext* ctx) {
    AddService(*ctx, new IApplicationFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
