#include "horizon/services/fssrv/filesystem_proxy.hpp"

namespace Hydra::Horizon::Services::FsSrv {

void IFileSystemProxy::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::FsSrv
