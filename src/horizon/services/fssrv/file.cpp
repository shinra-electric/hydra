#include "horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFile, 0, Read)

struct ReadIn {
    u32 option;
    u32 pad;
    i64 offset;
    u64 read_size;
};

void IFile::Read(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadIn>();
    ASSERT_DEBUG(in.offset >= 0, HorizonServices, "Offset ({}) must be >= 0",
                 in.offset);

    // TODO: option and pad

    usize size;
    auto file = open_file(path, size);
    ASSERT_DEBUG(in.read_size <= size, HorizonServices,
                 "Reading {} bytes, but file has a size of only {} bytes",
                 in.read_size, size);

    file.seekg(in.offset, std::ios::beg);
    // TODO: should be handled differently
    file.read(
        reinterpret_cast<char*>(writers.recv_buffers_writers[0].GetBase()),
        in.read_size);
    // writers.recv_buffers_writers[0].Write(file., in.read_size);

    writers.writer.Write(in.read_size);
}

} // namespace Hydra::Horizon::Services::Fssrv
