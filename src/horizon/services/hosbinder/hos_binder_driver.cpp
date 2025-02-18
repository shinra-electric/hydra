#include "horizon/services/hosbinder/hos_binder_driver.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::HosBinder {

DEFINE_SERVICE_COMMAND_TABLE(IHOSBinderDriver, 0, TransactParcel, 1,
                             AdjustRefcount)

enum class BinderResult : i32 {
    Success = 0,
    PermissionDenied = -1,
    NameNotFound = -2,
    WouldBlock = -11,
    NoMemory = -12,
    AlreadyExists = -17,
    NoInit = -19,
    BadValue = -22,
    DeadObject = -32,
    InvalidOperation = -38,
    NotEnoughData = -61,
    UnknownTransaction = -74,
    BadIndex = -75,
    TimedOut = -110,
    FdsNotAllowed = INT32_MIN + 7,
    FailedTransaction = INT32_MIN + 2,
    BadType = INT32_MIN + 1,
};

// TODO: define these somewhere else
struct NvFence {
    u32 id;
    u32 value;
};

struct NvMultiFence {
    u32 num_fences;
    NvFence fences[4];
};

struct BqBufferOutput {
    u32 width;
    u32 height;
    u32 transformHint;
    u32 numPendingBuffers;
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

    BinderResult b_result = BinderResult::Success;
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
    case TransactCode::QueueBuffer: {
        LOG_WARNING(HorizonServices, "QueueBuffer not implemented");

        // Buffer output
        writer.Write<BqBufferOutput>({
            .width = 0,
            .height = 0,
            .transformHint = 0,
            .numPendingBuffers = 0,
        });

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

    // Result
    writer.Write(b_result);

    parcel_out->data_size = writer.GetWrittenSize() - written_begin;
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
