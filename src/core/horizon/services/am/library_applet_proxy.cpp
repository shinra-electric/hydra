#include "core/horizon/services/am/library_applet_proxy.hpp"

#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/library_applet_self_accessor.hpp"
#include "core/horizon/services/am/process_winding_controller.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 10,
                             GetProcessWindingController, 11,
                             GetLibraryAppletCreator, 20,
                             OpenLibraryAppletSelfAccessor, 1000,
                             GetDebugFunctions)

result_t ILibraryAppletProxy::GetCommonStateGetter(RequestContext* ctx) {
    AddService(*ctx, new ICommonStateGetter());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetSelfController(RequestContext* ctx) {
    AddService(*ctx, new ISelfController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetWindowController(RequestContext* ctx) {
    AddService(*ctx, new IWindowController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetAudioController(RequestContext* ctx) {
    AddService(*ctx, new IAudioController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetDisplayController(RequestContext* ctx) {
    AddService(*ctx, new IDisplayController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetProcessWindingController(RequestContext* ctx) {
    AddService(*ctx, new IProcessWindingController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetLibraryAppletCreator(RequestContext* ctx) {
    AddService(*ctx, new ILibraryAppletCreator());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletProxy::OpenLibraryAppletSelfAccessor(RequestContext* ctx) {
    AddService(*ctx, new ILibraryAppletSelfAccessor());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetDebugFunctions(RequestContext* ctx) {
    AddService(*ctx, new IDebugFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
