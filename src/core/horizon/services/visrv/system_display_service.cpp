#include "core/horizon/services/visrv/system_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

DEFINE_SERVICE_COMMAND_TABLE(ISystemDisplayService, 2207, SetLayerVisibility,
                             2312, CreateStrayLayer, 3200, GetDisplayMode)

void ISystemDisplayService::GetDisplayMode(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::ViSrv
