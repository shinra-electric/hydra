#include "core/horizon/services/am/application_proxy.hpp"

#include "core/horizon/services/am/application_functions.hpp"
#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 11,
                             GetLibraryAppletCreator, 20,
                             GetApplicationFunctions, 1000, GetDebugFunctions)

result_t IApplicationProxy::GetCommonStateGetter(RequestContext* ctx) {
    AddService(*ctx, new ICommonStateGetter());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetSelfController(RequestContext* ctx) {
    AddService(*ctx, new ISelfController());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetWindowController(RequestContext* ctx) {
    AddService(*ctx, new IWindowController());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetAudioController(RequestContext* ctx) {
    AddService(*ctx, new IAudioController());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetDisplayController(RequestContext* ctx) {
    AddService(*ctx, new IDisplayController());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetLibraryAppletCreator(RequestContext* ctx) {
    AddService(*ctx, new ILibraryAppletCreator());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetApplicationFunctions(RequestContext* ctx) {
    AddService(*ctx, new IApplicationFunctions());
    return RESULT_SUCCESS;
}

result_t IApplicationProxy::GetDebugFunctions(RequestContext* ctx) {
    AddService(*ctx, new IDebugFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
