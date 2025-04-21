#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"

namespace Hydra::Horizon {

struct ParcelData {
    u32 unknown0;
    u32 unknown1;
    u32 binder_id;
    u32 unknown2[3];
    u64 str;
    u64 unknown3;
};

struct Parcel {
    u32 data_size;
    u32 data_offset;
    u32 objects_size;
    u32 objects_offset;
};

struct ParcelFlattenedObject {
    i32 size;
    i32 fd_count;
};

struct DisplayBuffer {
    bool initialized = false;
    bool queued = false;
    HW::TegraX1::GPU::NvGraphicsBuffer buff;
};

constexpr usize MAX_BINDER_BUFFER_COUNT = 3; // TODO: what should this be?

struct DisplayBinder {
  public:
    // TODO: make private
    u32 weak_ref_count = 0;
    u32 strong_ref_count = 0;

    DisplayBinder() : event(new Kernel::Event(true)) {}

    void AddBuffer(i32 slot, HW::TegraX1::GPU::NvGraphicsBuffer buff);
    i32 GetAvailableSlot();
    void QueueBuffer(i32 slot);
    i32 ConsumeBuffer();

    // Getters
    HW::TegraX1::GPU::NvGraphicsBuffer& GetBuffer(i32 slot) {
        return buffers[slot].buff;
    }

    const Kernel::HandleWithId<Kernel::Event>& GetEvent() const {
        return event;
    }

  private:
    Kernel::HandleWithId<Kernel::Event> event;

    DisplayBuffer buffers[MAX_BINDER_BUFFER_COUNT]; // TODO: what should be the
                                                    // max number of buffers?
    u32 buffer_count = 0;
    std::queue<i32> queued_buffers;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
};

class DisplayDriver {
  public:
    u32 AddBinder() {
        u32 id = binders.size();
        binders.push_back(new DisplayBinder());

        return id;
    }

    // Getters
    DisplayBinder& GetBinder(u32 id) { return *binders[id]; }

  private:
    std::vector<DisplayBinder*> binders;
};

} // namespace Hydra::Horizon
