#include "horizon/services/fssrv/filesystem_proxy.hpp"

#include "horizon/services/fssrv/file.hpp"
#include "horizon/services/fssrv/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 1, SetCurrentProcess, 18,
                             OpenSdCardFileSystem, 200,
                             OpenDataStorageByProgramId, 1005,
                             GetGlobalAccessLogMode)

void IFileSystemProxy::OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS) {
    add_service(new IFileSystem());
}

void IFileSystemProxy::OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS) {
    const auto program_id = readers.reader.Read<u64>();
    LOG_DEBUG(HorizonServices, "Program ID: {}", program_id);

    // TODO: what to do with program ID?

    add_service(new IStorage("/rom/romFS"));
}

void IFileSystemProxy::GetGlobalAccessLogMode(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);

    // TODO: what should this be?
    writers.writer.Write<u32>(0);
}

} // namespace Hydra::Horizon::Services::Fssrv
