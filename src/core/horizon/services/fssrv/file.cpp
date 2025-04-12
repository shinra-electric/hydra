#include "core/horizon/services/fssrv/file.hpp"

#include "core/horizon/filesystem/file.hpp"
#include "core/horizon/filesystem/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

struct ReadIn {
    u32 option;
    u32 pad;
    i64 offset;
    u64 read_size;
};

} // namespace

void IFile::Read(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadIn>();

    // TODO: option

    ReadImpl(writers.recv_buffers_writers[0].GetBase(), in.offset,
             in.read_size);

    writers.writer.Write(in.read_size);
}

} // namespace Hydra::Horizon::Services::Fssrv
