#include "horizon/services/hosbinder/hos_binder_driver.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::HosBinder {

DEFINE_SERVICE_COMMAND_TABLE(IHOSBinderDriver, 0, TransactParcel, 1,
                             AdjustRefcount)

// TODO: define these somewhere else
struct NvFence {
    u32 id;
    u32 value;
};

struct NvMultiFence {
    u32 num_fences;
    NvFence fences[4];
};

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

    auto& writer = writers.recv_buffers_writers[0];

    auto parcel_out = writer.Write<Parcel>({.data_offset = sizeof(Parcel)});
    usize written_begin = writer.GetWrittenSize();

    switch (in.code) {
    case TransactCode::DequeueBuffer: {
        LOG_WARNING(HorizonServices, "DequeueBuffer not implemented");

        // HACK
        writer.Write((i32)0);
        writer.Write((i32)1);

        // Flattened object
        ParcelFlattenedObject flattened_obj = {
            .size = sizeof(NvMultiFence),
            .fd_count = 0,
        };
        writer.Write(flattened_obj);

        // NvMultiFence
        NvMultiFence nv_multi_fence = {
            .num_fences = 0,
        };
        writer.Write(nv_multi_fence);

        break;
    }
    case TransactCode::Connect: {
        LOG_WARNING(HorizonServices, "Connect not implemented");

        // HACK
        u64 arr[16] = {0};
        writer.Write(arr, 16);

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown code {}", in.code);
        break;
    }

    parcel_out->data_size = writer.GetWrittenSize() - written_begin;

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
