#include "horizon/services/fssrv/file.hpp"

#include "horizon/filesystem/file.hpp"
#include "horizon/filesystem/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

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

    auto file = Filesystem::Filesystem::GetInstance().GetFile(path);
    usize size;
    file->Open(size);
    ASSERT_DEBUG(in.read_size <= size, HorizonServices,
                 "Reading {} bytes, but file has a size of only {} bytes",
                 in.read_size, size);

    auto& stream = file->GetStream();

    stream.seekg(in.offset, std::ios::beg);
    stream.read(
        reinterpret_cast<char*>(writers.recv_buffers_writers[0].GetBase()),
        in.read_size);

    file->Close();

    writers.writer.Write(in.read_size);
}

} // namespace Hydra::Horizon::Services::Fssrv
