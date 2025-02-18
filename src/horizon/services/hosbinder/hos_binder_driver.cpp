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

struct InputBuffer {
    u32 magic;
    u32 width;
    u32 height;
    u32 stride;
    u32 format;
    u32 usage;

    u32 pid;
    u32 refcount;

    u32 num_fds;
    u32 num_words;
};

// TODO: move to a separate file

struct Surface {
    u32 width;
    u32 height;
    /*ColorFormat*/ u32 color_format;
    /*Layout*/ u32 layout;
    u32 pitch;
    u32 unused; // usually this field contains the nvmap handle, but it's
                // completely unused/overwritten during marshalling
    u32 offset;
    /*Kind*/ u32 kind;
    u32 block_height_log2;
    /*DisplayScanFormat*/ u32 scan;
    u32 second_field_offset;
    u64 flags;
    u64 size;
    u32 unk[6]; // compression related
};

struct GraphicsBuffer {
    i32 unk0;       // -1
    i32 nvmap_id;   // nvmap object id
    u32 unk2;       // 0
    u32 magic;      // 0xDAFFCAFF
    u32 pid;        // 42
    u32 type;       // ?
    u32 usage;      // GRALLOC_USAGE_* bitmask
    u32 format;     // PIXEL_FORMAT_*
    u32 ext_format; // copy of the above (in most cases)
    u32 stride;     // in pixels!
    u32 total_size; // in bytes
    u32 num_planes; // usually 1
    u32 unk12;      // 0
    Surface planes[3];
    u64 unused; // official sw writes a pointer to bookkeeping data here, but
                // it's otherwise completely unused/overwritten during
                // marshalling
};

void IHOSBinderDriver::TransactParcel(REQUEST_COMMAND_PARAMS) {
    auto& reader = readers.send_buffers_readers[0];
    auto& writer = writers.recv_buffers_writers[0];

    auto in = readers.reader.Read<TransactParcelIn>();
    auto parcel_in = reader.Read<Parcel>();
    reader.JumpToOffset(parcel_in.data_offset);

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
    case TransactCode::SetPreallocatedBuffer: {
        LOG_WARNING(HorizonServices, "SetPreallocatedBuffer not implemented");

        // Interface name
        reader.Read<i32>(); // 0x100

        i32 interface_name_len = reader.Read<i32>() + 1;
        char interface_name[interface_name_len];
        for (i32 i = 0; i < interface_name_len; i++) {
            interface_name[i] = reader.Read<u16>();
        }

        // Alignment
        if (interface_name_len % 2 != 0)
            reader.Read<u16>();

        LOG_DEBUG(HorizonServices, "Interface name: {}",
                  reinterpret_cast<const char*>(interface_name));

        // Slot
        i32 slot = reader.Read<i32>();
        LOG_DEBUG(HorizonServices, "Slot: {}", slot);

        // Input buffer
        bool has_input_buffer = reader.Read<i32>();
        if (!has_input_buffer) {
            LOG_WARNING(HorizonServices, "No input buffer");
            break;
        }

        reader.Read<i32>(); // len
        reader.Read<i32>(); // fd_count

        InputBuffer input_buffer = reader.Read<InputBuffer>();
        GraphicsBuffer graphics_buffer = reader.Read<GraphicsBuffer>();

        LOG_DEBUG(HorizonServices, "Framebuffer size: {}",
                  graphics_buffer.total_size);

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
