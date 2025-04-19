#include "core/horizon/services/am/storage_accessor.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IStorageAccessor, 0, GetSize, 10, Write, 11, Read)

void IStorageAccessor::GetSize(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write<i64>(data.GetSize());
}

void IStorageAccessor::Write(REQUEST_COMMAND_PARAMS) {
    const auto offset = readers.reader.Read<i64>();
    ASSERT_DEBUG(offset >= 0, HorizonServices, "Offset must be >= 0");

    // TODO: correct?
    const usize size = data.GetSize() - offset;

    const auto ptr = readers.send_buffers_readers[0].ReadPtr<u8>(size);
    memcpy(data.GetPtrU8() + offset, ptr, size);
}

void IStorageAccessor::Read(REQUEST_COMMAND_PARAMS) {
    const auto offset = readers.reader.Read<i64>();
    ASSERT_DEBUG(offset >= 0, HorizonServices, "Offset must be >= 0");

    // TODO: correct?
    const usize size = data.GetSize() - offset;

    writers.recv_buffers_writers[0].Write<u8>(data.GetPtrU8() + offset, size);
}

} // namespace Hydra::Horizon::Services::Am
