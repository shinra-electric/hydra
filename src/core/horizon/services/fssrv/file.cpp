#include "core/horizon/services/fssrv/file.hpp"

#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/filesystem/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

struct ReadWriteIn {
    u32 option; // TODO: enum
    u32 pad;
    i64 offset;
    u64 size;
};

} // namespace

void IFile::Read(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadWriteIn>();

    // TODO: option

    usize size = in.size;
    ReadImpl(writers.recv_buffers_writers[0].GetBase(), in.offset, size);

    writers.writer.Write(size);
}

void IFile::Write(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadWriteIn>();

    // TODO: option

    WriteImpl(readers.send_buffers_readers[0].GetBase(), in.offset, in.size);
}

} // namespace Hydra::Horizon::Services::Fssrv
