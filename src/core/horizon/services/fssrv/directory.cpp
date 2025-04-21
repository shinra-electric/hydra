#include "core/horizon/services/fssrv/directory.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"

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
    if (entry_index >= directory->GetEntries().size()) {
        writers.writer.Write<i32>(0);
        return;
    }

    auto& writer = writers.recv_buffers_writers[0];

    // TODO: respect filter flags
    u32 i = 0;
    for (const auto& [path, entry] : directory->GetEntries()) {
        // TODO: find a better way to index
        if (i < entry_index)
            continue;

        FsDirectoryEntry e{};
        memcpy(e.name, path.c_str(), path.size());
        e.type =
            (entry->IsDirectory() ? EntryType::Directory : EntryType::File);
        if (!entry->IsDirectory())
            e.file_size = static_cast<Filesystem::FileBase*>(entry)->GetSize();
        else
            e.file_size = 0;

        writer.Write(e);

        entry_index++;
        i++;

        // Check if the writer has enough space to write the next entry
        if (writer.GetWrittenSize() + sizeof(FsDirectoryEntry) >
            writer.GetSize())
            break;
    }

    writers.writer.Write<i32>(writer.GetWrittenSize() /
                              sizeof(FsDirectoryEntry));
}

} // namespace Hydra::Horizon::Services::Fssrv
