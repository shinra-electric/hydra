#include "core/horizon/services/hosbinder/hos_binder_driver.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/os.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"

// TODO: this whole thing needs a rewrite

namespace Hydra::Horizon::Services::HosBinder {

namespace {

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

void read_interface_name(Reader& reader) {
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
}

} // namespace

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

#include "common/logging/log.hpp"

namespace Hydra::Horizon::Services::HosBinder {

namespace {

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
    u32 transform_hint;
    u32 num_pending_buffers;
};

struct TransactParcelIn {
    i32 binder_id;
    TransactCode code;
    u32 flags;
};

enum class BinderType : i32 {
    Weak = 0,
    Strong = 1,
};

struct AdjustRefcountIn {
    i32 binder_id;
    i32 addval;
    BinderType type;
};

struct GetNativeHandleIn {
    i32 binder_id;
    u32 code; // TODO: should this be TransactCode?
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IHOSBinderDriver, 0, TransactParcel, 1,
                             AdjustRefcount, 2, GetNativeHandle)

void IHOSBinderDriver::TransactParcel(REQUEST_COMMAND_PARAMS) {
    auto& reader = readers.send_buffers_readers[0];
    auto& writer = writers.recv_buffers_writers[0];

    auto in = readers.reader.Read<TransactParcelIn>();
    LOG_DEBUG(HorizonServices, "Code: {}", in.code);

    auto parcel_in = reader.Read<Parcel>();
    reader.Seek(parcel_in.data_offset);

    auto parcel_out = writer.Write<Parcel>({.data_offset = sizeof(Parcel)});
    usize written_begin = writer.GetWrittenSize();

    // Binder
    auto& binder = OS::GetInstance().GetDisplayDriver().GetBinder(in.binder_id);

    // Dispatch
    BinderResult b_result = BinderResult::Success;
    switch (in.code) {
    case TransactCode::RequestBuffer: {
        read_interface_name(reader);

        i32 slot = reader.Read<i32>();
        if (slot > MAX_BINDER_BUFFER_COUNT) {
            writer.Write<u32>(0x0);
            break;
        }

        // Buffer
        writer.Write<u32>(0x1);                   // TODO: correct?
        writer.Write<i32>(sizeof(GraphicBuffer)); // len
        writer.Write<i32>(0);                     // fd_count

        const auto& buffer = binder.GetBuffer(slot);
        writer.Write(buffer);

        break;
    }
    case TransactCode::DequeueBuffer: {
        i32 slot = binder.GetAvailableSlot();

        writer.Write(slot);
        // HACK
        writer.Write<i32>(1);

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
        LOG_NOT_IMPLEMENTED(HorizonServices, "QueueBuffer");

        read_interface_name(reader);

        // Slot
        i32 slot = reader.Read<i32>();

        // TODO: input

        binder.QueueBuffer(slot);

        // Buffer output
        // TODO
        writer.Write<BqBufferOutput>({
            .width = 0,
            .height = 0,
            .transform_hint = 0,
            .num_pending_buffers = 0,
        });

        break;
    }
    case TransactCode::Query: {
        read_interface_name(reader);

        const auto what = reader.Read<i32>(); // TODO: enum
        LOG_NOT_IMPLEMENTED(HorizonServices, "Query (what: {})", what);

        i32 value = 0; // HACK

        writer.Write(value);

        break;
    }
    case TransactCode::Connect: {
        struct {
            u32 width;
            u32 height;
            u32 transform_hint;
            u32 num_pending_buffers;
        } output{
            .width = 1280,            // TODO: dont' hardcode
            .height = 720,            // TODO: dont' hardcode
            .transform_hint = 0,      // HACK
            .num_pending_buffers = 0, // HACK
        };

        writer.Write(output);

        break;
    }
    case TransactCode::SetPreallocatedBuffer: {
        read_interface_name(reader);

        // Slot
        i32 slot = reader.Read<i32>();

        // Input buffer
        bool has_input_buffer = reader.Read<i32>();
        if (!has_input_buffer) {
            LOG_WARN(HorizonServices, "No input buffer");
            break;
        }

        reader.Read<i32>(); // len
        reader.Read<i32>(); // fd_count

        auto buffer = reader.Read<GraphicBuffer>();

        // Debug
        const auto& plane = buffer.nv_buffer.planes[0];
        LOG_DEBUG(HorizonServices,
                  "width: {}, height: {}, color_format: {}, "
                  "layout: {}, pitch: 0x{:08x}, "
                  "unused: 0x{:08x}, offset: 0x{:08x}, kind: "
                  "{}, size: 0x{:08x}",
                  plane.width, plane.height, plane.color_format, plane.layout,
                  plane.pitch, plane.unused, plane.offset, plane.kind,
                  plane.size);

        binder.AddBuffer(slot, buffer);

        break;
    }
    default:
        LOG_WARN(HorizonServices, "Unknown code {}", in.code);
        break;
    }

    // Result
    writer.Write(b_result);

    parcel_out->data_size = writer.GetWrittenSize() - written_begin;
}

void IHOSBinderDriver::AdjustRefcount(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<AdjustRefcountIn>();

    auto& binder = OS::GetInstance().GetDisplayDriver().GetBinder(in.binder_id);

    switch (in.type) {
    case BinderType::Weak:
        binder.weak_ref_count += in.addval;
        break;
    case BinderType::Strong:
        binder.strong_ref_count += in.addval;
        break;
    }
}

void IHOSBinderDriver::GetNativeHandle(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<GetNativeHandleIn>();

    writers.copy_handles_writer.Write(OS::GetInstance()
                                          .GetDisplayDriver()
                                          .GetBinder(in.binder_id)
                                          .GetEvent()
                                          .id);
}

} // namespace Hydra::Horizon::Services::HosBinder
