#include "horizon/services/settings/system_settings_server.hpp"

namespace Hydra::Horizon::Services::Settings {

void ISystemSettingsServer::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Settings
