#pragma once

#include "core/horizon/applets/const.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::applets {

class AppletBase {
  public:
    AppletBase(const LibraryAppletMode mode_)
        : mode{mode_}, state_changed_event(new kernel::Event()) {}
    virtual ~AppletBase() {
        if (thread) {
            // TODO: join?
            thread->join();
            delete thread;
        }
    }

    void PushInData(const sized_ptr data) { in_data.push(data); }

    sized_ptr PopOutData() {
        ASSERT(!out_data.empty(), Services, "No output data");
        const auto data = out_data.front();
        out_data.pop();

        return data;
    }

    void Start();

    handle_id_t GetStateChangedEventID() const {
        return state_changed_event.id;
    }

    result_t GetResult() const { return result; }

  protected:
    LibraryAppletMode mode;

    virtual result_t Run() = 0;

    // Helpers
    template <typename T> T PopInData() {
        ASSERT(!in_data.empty(), Services, "No input data");
        const auto data = in_data.front();
        ASSERT(data.GetSize() >= sizeof(T), Services,
               "Not enough space ({} < {})", data.GetSize(), sizeof(T));
        in_data.pop();

        return *reinterpret_cast<T*>(data.GetPtr());
    }

    template <typename T> void PushOutData(const T& data) {
        auto ptr = malloc(sizeof(T));
        memcpy(ptr, data.GetPtrU8(), sizeof(T));
        out_data.push(sized_ptr(ptr, sizeof(T)));
    }

  private:
    kernel::HandleWithId<kernel::Event> state_changed_event;
    std::thread* thread{nullptr};

    std::queue<sized_ptr> in_data;
    std::queue<sized_ptr> out_data;

    result_t result{RESULT_SUCCESS};
};

} // namespace hydra::horizon::applets
