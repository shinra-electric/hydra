#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"

namespace hydra::horizon {

struct GraphicBufferHeader {
    u32 magic;
    u32 width;
    u32 height;
    u32 stride;
    u32 format;
    u32 usage;

    u32 pid;
    u32 refcount;

    u32 fd_count;
    u32 int_count;
};

struct GraphicBuffer {
    GraphicBufferHeader header;
    hw::tegra_x1::gpu::NvGraphicsBuffer nv_buffer;
};

struct DisplayBuffer {
    bool initialized = false;
    bool queued = false;
    GraphicBuffer buffer;
};

constexpr usize MAX_BINDER_BUFFER_COUNT = 3; // TODO: what should this be?

struct DisplayBinder {
  public:
    // TODO: make private
    u32 weak_ref_count = 0;
    u32 strong_ref_count = 0;

    DisplayBinder() : event(new kernel::Event(true)) {}

    void AddBuffer(i32 slot, const GraphicBuffer& buff);
    i32 GetAvailableSlot();
    void QueueBuffer(i32 slot);
    i32 ConsumeBuffer();

    // Getters
    const GraphicBuffer& GetBuffer(i32 slot) const {
        return buffers[slot].buffer;
    }

    const kernel::HandleWithId<kernel::Event>& GetEvent() const {
        return event;
    }

  private:
    kernel::HandleWithId<kernel::Event> event;

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

} // namespace hydra::horizon
