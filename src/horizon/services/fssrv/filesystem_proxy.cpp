#include "horizon/services/fssrv/filesystem_proxy.hpp"

#include "horizon/services/fssrv/file.hpp"
#include "horizon/services/fssrv/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 18, OpenSdCardFileSystem, 200,
                             OpenDataStorageByProgramId)

void IFileSystemProxy::OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS) {
    add_service(new IFileSystem());
}

void IFileSystemProxy::OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS) {
    const auto program_id = readers.reader.Read<u64>();
    LOG_DEBUG(HorizonServices, "Program ID: {}", program_id);

    // TODO: what to do with program ID?

    add_service(new IFile("/rom/romFS",
                          FileFlags::Read)); // TODO: are the flags correct?
}

} // namespace Hydra::Horizon::Services::Fssrv
