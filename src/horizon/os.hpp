#pragma once

#include "horizon/kernel.hpp"

namespace Hydra::HW::Display {
class DisplayBase;
}

namespace Hydra::Horizon {

namespace Services::Sm {
class IUserInterface;
}

// TODO: move to a separate file
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

struct TextureDescriptor {
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

struct DisplayBuffer {
    bool initialized = false;
    bool queued = false;
    TextureDescriptor tex;
};

struct DisplayBinder {
  public:
    // TODO: make private
    u32 weak_ref_count = 0;
    u32 strong_ref_count = 0;

    void AddBuffer(i32 slot, TextureDescriptor tex) {
        buffers[slot].initialized = true;
        buffers[slot].tex = tex;
        buffer_count++;
    }

    i32 GetAvailableSlot() {
        // Wait for a slot to become available
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cv.wait(lock,
                      [&] { return queued_buffers.size() != buffer_count; });

        // Find an available slot
        for (i32 i = 0; i < 8; i++) {
            if (!buffers[i].initialized) {
                return i;
            }
        }

        // Should be unreachable
        throw std::runtime_error("No available slots");
    }

    void QueueBuffer(i32 slot) {
        queue_mutex.lock();
        queued_buffers.push(slot);
        buffers[slot].queued = true;
        queue_mutex.unlock();
        queue_cv.notify_all(); // TODO: all?
    }

    i32 ConsumeBuffer() {
        // Wait for a buffer to become available
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cv.wait(lock, [&] { return !queued_buffers.empty(); });

        // Get the first queued buffer
        queue_mutex.lock();
        i32 slot = queued_buffers.front();
        queued_buffers.pop();
        buffers[slot].queued = false;
        queue_mutex.unlock();
        queue_cv.notify_all(); // TODO: all?

        return slot;
    }

    // Getters
    TextureDescriptor& GetBuffer(i32 slot) { return buffers[slot].tex; }

  private:
    DisplayBuffer buffers[8]; // TODO: what should be the max number of buffers?
    u32 buffer_count = 0;
    std::queue<i32> queued_buffers;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
};

class DisplayBinderManager {
  public:
    u32 AddBinder() {
        u32 id = binders.size();
        binders.push_back(new DisplayBinder());

        return id;
    }

    DisplayBinder& GetBinder(u32 id) { return *binders[id]; }

  private:
    std::vector<DisplayBinder*> binders;
};

class OS {
  public:
    static OS& GetInstance();

    OS(HW::Bus& bus, HW::TegraX1::CPU::MMUBase* mmu);
    ~OS();

    void LoadROM(Rom* rom, HW::TegraX1::CPU::ThreadBase* thread);

    // Getters
    Kernel& GetKernel() { return kernel; }

    DisplayBinderManager& GetDisplayBinderManager() {
        return display_binder_manager;
    }

  private:
    Kernel kernel;

    // Services
    Services::Sm::IUserInterface* sm_user_interface;

    // Managers
    DisplayBinderManager display_binder_manager;
};

} // namespace Hydra::Horizon
