#include "horizon/services/hosbinder/hos_binder_driver.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::HosBinder {

DEFINE_SERVICE_COMMAND_TABLE(IHOSBinderDriver, 0, TransactParcel, 1,
                             AdjustRefcount)

enum class TransactCode : u32 {
    RequestBuffer = 1,
    SetBufferCount,
    DequeueBuffer,
    DetachBuffer,
    DetachNextBuffer,
    AttachBuffer,
    QueueBuffer,
    CancelBuffer,
    Query,
    Connect,
    Disconnect,
    SetSidebandStream,
    AllocateBuffers,
    SetPreallocatedBuffer,
};

struct TransactParcelIn {
    i32 binder_id;
    TransactCode code;
    u32 flags;
};

void IHOSBinderDriver::TransactParcel(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<TransactParcelIn>();

    auto parcel_out = writers.recv_buffers_writers[0].Write<Parcel>(
        {.data_offset = sizeof(Parcel)});

    switch (in.code) {
    case TransactCode::DequeueBuffer: {
        LOG_DEBUG(HorizonServices, "DequeueBuffer not implemented");

        // HACK
        u64 arr[16] = {0};
        writers.recv_buffers_writers[0].Write<u64>(arr, 16);
        parcel_out->data_size = 16 * sizeof(u64);

        break;
    }
    case TransactCode::Connect: {
        LOG_DEBUG(HorizonServices, "Connect not implemented");

        // HACK
        u64 arr[16] = {0};
        writers.recv_buffers_writers[0].Write<u64>(arr, 16);
        parcel_out->data_size = 16 * sizeof(u64);

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown code {}", in.code);
        break;
    }

    // Parcel
    // auto parcel = readers.send_buffers_readers[0].Read<Parcel>();
    // writers.recv_buffers_writers[0].Write(parcel);
}

enum class BinderType : i32 {
    Weak = 0,
    Strong = 1,
};

struct AdjustRefcountIn {
    i32 binder_id;
    i32 addval;
    BinderType type;
};

void IHOSBinderDriver::AdjustRefcount(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<AdjustRefcountIn>();
    auto& binder =
        OS::GetInstance().GetDisplayBinderManager().GetBinder(in.binder_id);
    switch (in.type) {
    case BinderType::Weak:
        binder.weak_ref_count += in.addval;
        break;
    case BinderType::Strong:
        binder.strong_ref_count += in.addval;
        break;
    }
}

} // namespace Hydra::Horizon::Services::HosBinder

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::HosBinder::TransactCode,
                       RequestBuffer, "request buffer", SetBufferCount,
                       "set buffer count", DequeueBuffer, "dequeue buffer",
                       DetachBuffer, "detach buffer", DetachNextBuffer,
                       "detach next buffer", AttachBuffer, "attach buffer",
                       QueueBuffer, "queue buffer", CancelBuffer,
                       "cancel buffer", Query, "query", Connect, "connect",
                       Disconnect, "disconnect", SetSidebandStream,
                       "set sideband stream", AllocateBuffers,
                       "allocate buffers", SetPreallocatedBuffer,
                       "set preallocated buffer")
