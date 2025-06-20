#include "core/horizon/services/am/library_applet_self_accessor.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletSelfAccessor, 11,
                             GetLibraryAppletInfo)

result_t
ILibraryAppletSelfAccessor::GetLibraryAppletInfo(LibraryAppletInfo* out_info) {
    LOG_FUNC_STUBBED(Services);

    // HACK: hardcoded for Mii Edit
    *out_info = {
        .id = AppletId::LibraryAppletMiiEdit,
        .mode = LibraryAppletMode::AllForeground,
    };
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
