#include "core/horizon/display_driver.hpp"

namespace hydra::horizon {

void DisplayBinder::AddBuffer(i32 slot, const GraphicBuffer& buff) {
    buffers[slot].initialized = true;
    buffers[slot].buffer = buff;
    buffer_count++;
}

i32 DisplayBinder::GetAvailableSlot() {
    // Wait for a slot to become available
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_cv.wait(lock, [&] { return queued_buffers.size() != buffer_count; });

    // Find an available slot
    i32 slot = -1;
    for (i32 i = 0; i < MAX_BINDER_BUFFER_COUNT; i++) {
        if (buffers[i].initialized && !buffers[i].queued) {
            if (slot == -1) {
                // Consume the first slot
                slot = i;
            } else {
                // We know that there is another available slot, so we can
                // keep the event signaled
                return slot;
            }
        }
    }

    // If we reach here, it means that there won't be a slot available the
    // next time, so clear the event
    event.handle->Clear();

    // TODO: remove this?
    if (slot == -1)
        LOG_ERROR(Horizon, "No available slots");

    return slot;
}

void DisplayBinder::QueueBuffer(i32 slot) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    queued_buffers.push(slot);
    buffers[slot].queued = true;
    queue_cv.notify_all();
}

i32 DisplayBinder::ConsumeBuffer() {
    // Wait for a buffer to become available
    std::unique_lock<std::mutex> lock(queue_mutex);
    // TODO: should there be a timeout?
    queue_cv.wait_for(lock, std::chrono::nanoseconds(67 * 1000 * 1000),
                      [&] { return !queued_buffers.empty(); });

    if (queued_buffers.empty())
        return -1;

    // Get the first queued buffer
    i32 slot = queued_buffers.front();
    queued_buffers.pop();
    buffers[slot].queued = false;
    queue_cv.notify_all();

    // Signal event
    event.handle->Signal();

    return slot;
}

} // namespace hydra::horizon
