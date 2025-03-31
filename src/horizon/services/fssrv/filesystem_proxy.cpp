#include "horizon/services/fssrv/filesystem_proxy.hpp"

#include "horizon/services/fssrv/file.hpp"
#include "horizon/services/fssrv/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 0, OpenFileSystem, 1,
                             SetCurrentProcess, 18, OpenSdCardFileSystem, 200,
                             OpenDataStorageByProgramId, 1005,
                             GetGlobalAccessLogMode)

void IFileSystemProxy::OpenFileSystem(REQUEST_COMMAND_PARAMS) {
    // TODO: correct?
    auto path = readers.send_buffers_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    add_service(new IFileSystem(/*path*/));
}

void IFileSystemProxy::OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS) {
    // TODO: correct?
    add_service(new IFileSystem(/*"/sdmc"*/));
}

void IFileSystemProxy::OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS) {
    const auto program_id = readers.reader.Read<u64>();
    LOG_DEBUG(HorizonServices, "Program ID: {}", program_id);

    // TODO: what to do with program ID?

    add_service(new IStorage("/rom/romFS"));
}

void IFileSystemProxy::GetGlobalAccessLogMode(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: what should this be?
    writers.writer.Write<u32>(0);
}

} // namespace Hydra::Horizon::Services::Fssrv
