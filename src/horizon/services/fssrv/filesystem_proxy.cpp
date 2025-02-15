#include "horizon/services/fssrv/filesystem_proxy.hpp"

namespace Hydra::Horizon::Services::Fssrv {

void IFileSystemProxy::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Fssrv
