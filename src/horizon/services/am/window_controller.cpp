#include "horizon/services/am/window_controller.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IWindowController, 1, GetAppletResourceUserId, 10,
                             AcquireForegroundRights)

} // namespace Hydra::Horizon::Services::Am
