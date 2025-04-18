#include "core/horizon/services/fssrv/directory.hpp"

#include "core/horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

struct FsDirectoryEntry {
    char name[MAX_PATH_SIZE];
    u8 pad[3];
    EntryType type : 8;
    u8 pad2[3];
    i64 file_size;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IDirectory, 0, Read)

void IDirectory::Read(REQUEST_COMMAND_PARAMS) {
    u64 index = 0;

    // TODO: respect filter flags
    auto& writer = writers.recv_buffers_writers[0];
    for (const auto& [path, entry] : directory->GetEntries()) {
        FsDirectoryEntry e{};
        memcpy(e.name, path.c_str(), path.size());
        e.type =
            (entry->IsDirectory() ? EntryType::Directory : EntryType::File);
        if (!entry->IsDirectory())
            e.file_size = static_cast<Filesystem::File*>(entry)->GetSize();
        else
            e.file_size = 0;

        writer.Write(e);
    }

    writers.writer.Write(writer.GetWrittenSize() / sizeof(FsDirectoryEntry));
}

} // namespace Hydra::Horizon::Services::Fssrv
