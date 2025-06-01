#pragma once

#include "core/horizon/applets/const.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::applets {

class AppletBase {
  public:
    AppletBase(const LibraryAppletMode mode_)
        : mode{mode_}, state_changed_event(new kernel::Event()),
          interactive_in_data_event(new kernel::Event(true)),
          interactive_out_data_event(new kernel::Event()) {}
    virtual ~AppletBase() {
        if (thread) {
            // TODO: join?
            thread->join();
            delete thread;
        }
    }

    // Data
    void PushInData(const sized_ptr data) { in_data.push(data); }

    sized_ptr PopOutData() {
        ASSERT(!out_data.empty(), Applets, "No output data");
        const auto data = out_data.front();
        out_data.pop();

        return data;
    }

    // Interactive data
    void PushInteractiveInData(const sized_ptr data) {
        interactive_in_data.push(data);
        interactive_in_data_event.handle->Signal();
    }

    sized_ptr PopInteractiveOutData() {
        // No need to wait for event, as the guest is responsible for that

        ASSERT(!interactive_out_data.empty(), Applets,
               "No interactive output data");
        const auto data = interactive_out_data.front();
        interactive_out_data.pop();

        return data;
    }

    void Start();

    handle_id_t GetStateChangedEventID() const {
        return state_changed_event.id;
    }

    handle_id_t GetInteractiveOutDataEventID() const {
        return interactive_out_data_event.id;
    }

    result_t GetResult() const { return result; }

  protected:
    LibraryAppletMode mode;

    virtual result_t Run() = 0;

    // Helpers

    // Data
    Reader PopInDataRaw() {
        ASSERT(!in_data.empty(), Applets, "No input data");
        const auto data = in_data.front();
        in_data.pop();

        return Reader(data.GetPtrU8(), data.GetSize());
    }

    template <typename T> T PopInData() {
        auto reader = PopInDataRaw();
        ASSERT(reader.GetSize() >= sizeof(T), Applets,
               "Not enough space ({} < {})", reader.GetSize(), sizeof(T));

        return reader.Read<T>();
    }

    void PushOutDataRaw(const sized_ptr ptr) { out_data.push(ptr); }

    template <typename T> void PushOutData(const T& data) {
        auto ptr = malloc(sizeof(T));
        memcpy(ptr, &data, sizeof(T));
        PushOutDataRaw(sized_ptr(ptr, sizeof(T)));
    }

    // Interactive data
    Reader PopInteractiveInDataRaw() {
        interactive_in_data_event.handle->Wait();

        ASSERT(!interactive_in_data.empty(), Applets, "No input data");
        const auto data = interactive_in_data.front();
        interactive_in_data.pop();

        return Reader(data.GetPtrU8(), data.GetSize());
    }

    template <typename T> T PopInteractiveInData() {
        auto reader = PopInteractiveInDataRaw();
        ASSERT(reader.GetSize() >= sizeof(T), Applets,
               "Not enough space ({} < {})", reader.GetSize(), sizeof(T));

        return reader.Read<T>();
    }

    void PushInteractiveOutDataRaw(const sized_ptr ptr) {
        interactive_out_data.push(ptr);
        interactive_out_data_event.handle->Signal();
    }

    template <typename T> void PushInteractiveOutData(const T& data) {
        auto ptr = malloc(sizeof(T));
        memcpy(ptr, &data, sizeof(T));
        PushInteractiveOutDataRaw(sized_ptr(ptr, sizeof(T)));
    }

  private:
    kernel::HandleWithId<kernel::Event> state_changed_event;
    kernel::HandleWithId<kernel::Event>
        interactive_in_data_event; // TODO: use a regular condition variable?
    kernel::HandleWithId<kernel::Event> interactive_out_data_event;

    std::thread* thread{nullptr};

    std::queue<sized_ptr> in_data;
    std::queue<sized_ptr> out_data;
    std::queue<sized_ptr> interactive_in_data;
    std::queue<sized_ptr> interactive_out_data;

    result_t result{RESULT_SUCCESS};
};

} // namespace hydra::horizon::applets
