#include "horizon/services/settings/system_settings_server.hpp"

namespace Hydra::Horizon::Services::Settings {

void ISystemSettingsServer::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Settings
