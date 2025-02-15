#include "horizon/services/hid/hid_server.hpp"

namespace Hydra::Horizon::Services::Hid {

void IHidServer::RequestImpl(Readers& readers, Writers& writers,
                             std::function<void(ServiceBase*)> add_service,
                             Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Hid
