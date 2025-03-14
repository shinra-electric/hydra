#include "horizon/services/fssrv/filesystem.hpp"

#include "horizon/services/fssrv/directory.hpp"
#include "horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 8, OpenFile, 9, OpenDirectory)

void IFileSystem::OpenFile(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto flags = readers.reader.Read<FileFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, flags: {}", path, flags);

    add_service(new IFile(path, flags));
}

void IFileSystem::OpenDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto filter_flags = readers.reader.Read<DirectoryFilterFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, filter flags: {}", path,
              filter_flags);

    add_service(new IDirectory(path, filter_flags));
}

} // namespace Hydra::Horizon::Services::Fssrv
