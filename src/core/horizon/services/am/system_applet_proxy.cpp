#include "core/horizon/services/am/system_applet_proxy.hpp"

#include "core/horizon/services/am/application_creator.hpp"
#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/global_state_controller.hpp"
#include "core/horizon/services/am/home_menu_functions.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/process_winding_controller.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ISystemAppletProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 10,
                             GetProcessWindingController, 11,
                             GetLibraryAppletCreator, 20, GetHomeMenuFunctions,
                             21, GetGlobalStateController, 22,
                             GetApplicationCreator, 1000, GetDebugFunctions)

result_t ISystemAppletProxy::GetCommonStateGetter(RequestContext* ctx) {
    AddService(*ctx, new ICommonStateGetter());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetSelfController(RequestContext* ctx) {
    AddService(*ctx, new ISelfController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetWindowController(RequestContext* ctx) {
    AddService(*ctx, new IWindowController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetAudioController(RequestContext* ctx) {
    AddService(*ctx, new IAudioController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetDisplayController(RequestContext* ctx) {
    AddService(*ctx, new IDisplayController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetProcessWindingController(RequestContext* ctx) {
    AddService(*ctx, new IProcessWindingController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetLibraryAppletCreator(RequestContext* ctx) {
    AddService(*ctx, new ILibraryAppletCreator());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetHomeMenuFunctions(RequestContext* ctx) {
    AddService(*ctx, new IHomeMenuFunctions());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetGlobalStateController(RequestContext* ctx) {
    AddService(*ctx, new IGlobalStateController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetApplicationCreator(RequestContext* ctx) {
    AddService(*ctx, new IApplicationCreator());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetDebugFunctions(RequestContext* ctx) {
    AddService(*ctx, new IDebugFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
