#include "horizon/services/fssrv/filesystem_proxy.hpp"

#include "horizon/services/fssrv/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 18, OpenSdCardFileSystem)

void IFileSystemProxy::OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS) {
    add_service(new IFileSystem());
}

} // namespace Hydra::Horizon::Services::Fssrv
