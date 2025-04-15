#include "core/horizon/services/fssrv/storage.hpp"

#include "core/horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

struct ReadIn {
    i64 offset;
    u64 read_size;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IStorage, 0, Read, 4, GetSize)

IStorage::IStorage(Filesystem::File* file_) : file{file_} { file->Open(); }

IStorage::~IStorage() { file->Close(); }

void IStorage::ReadImpl(u8* ptr, u64 offset, usize& size) {
    LOG_DEBUG(HorizonServices, "Offset: 0x{:08x}, size: 0x{:08x}", offset,
              size);

    ASSERT_DEBUG(offset >= 0, HorizonServices, "Offset ({}) must be >= 0",
                 offset);

    auto reader = file->CreateReader();
    if (size > reader.GetSize()) {
        LOG_WARNING(HorizonServices,
                    "Reading {} bytes, but file has a size of only {} bytes",
                    size, reader.GetSize());
        size = reader.GetSize();
    }

    reader.Seek(offset);
    reader.Read(ptr, size);
}

void IStorage::Read(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<ReadIn>();

    usize size = in.read_size;
    ReadImpl(writers.recv_buffers_writers[0].GetBase(), in.offset, size);
}

void IStorage::GetSize(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write(file->GetSize());
}

} // namespace Hydra::Horizon::Services::Fssrv
