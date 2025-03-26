#include "horizon/services/fssrv/storage.hpp"

#include "horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IStorage, 0, Read)

struct ReadIn {
    i64 offset;
    u64 read_size;
};

void IStorage::Read(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadIn>();

    ReadImpl(writers.recv_buffers_writers[0].GetBase(), in.offset,
             in.read_size);
}

void IStorage::ReadImpl(u8* ptr, u64 offset, u64 size) {
    LOG_DEBUG(HorizonServices, "Offset: 0x{:08x}, size: 0x{:08x}", offset,
              size);

    ASSERT_DEBUG(offset >= 0, HorizonServices, "Offset ({}) must be >= 0",
                 offset);

    auto file = Filesystem::Filesystem::GetInstance().GetFile(path);
    file->Open();

    auto reader = file->CreateReader();
    ASSERT_DEBUG(size <= reader.GetSize(), HorizonServices,
                 "Reading {} bytes, but file has a size of only {} bytes", size,
                 reader.GetSize());

    reader.Seek(offset);
    reader.Read(ptr, size);

    file->Close();
}

} // namespace Hydra::Horizon::Services::Fssrv
