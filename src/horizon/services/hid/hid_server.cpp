#include "horizon/services/hid/hid_server.hpp"

namespace Hydra::Horizon::Services::Hid {

void IHidServer::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Hid
