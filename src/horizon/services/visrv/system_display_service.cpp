#include "horizon/services/visrv/system_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void ISystemDisplayService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 2312: // CreateStrayLayer
        CmdCreateStrayLayer(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::ViSrv
