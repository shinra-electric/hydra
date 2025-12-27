#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/am/library_applet_controller.hpp"

namespace hydra::horizon::applets {

class AppletBase {
  public:
    AppletBase(services::am::LibraryAppletController& controller_)
        : controller{controller_} {}
    virtual ~AppletBase() {
        if (thread) {
            // TODO: join?
            thread->join();
            delete thread;
        }
    }

    void Start();

    result_t GetResult() const { return result; }

  protected:
    virtual result_t Run() = 0;

    // Helpers

    // Data
    io::MemoryStream PopInDataRaw() {
        auto data = controller.PopInData()->GetData();
        return io::MemoryStream(std::span(data.GetPtrU8(), data.GetSize()));
    }

    template <typename T>
    T PopInData() {
        auto stream = PopInDataRaw();
        ASSERT(stream.GetSize() >= sizeof(T), Applets,
               "Not enough space ({} < {})", stream.GetSize(), sizeof(T));

        return stream.Read<T>();
    }

    void PushOutDataRaw(const sized_ptr data) {
        controller.PushOutData(new services::am::IStorage(data));
    }

    template <typename T>
    void PushOutData(const T& data) {
        auto ptr = malloc(sizeof(T));
        memcpy(ptr, &data, sizeof(T));
        PushOutDataRaw(sized_ptr(ptr, sizeof(T)));
    }

    // Interactive data
    io::MemoryStream PopInteractiveInDataRaw() {
        // TODO: wait
        // controller.GetInteractiveInDataEvent()->Wait();
        auto data = controller.PopInteractiveInData()->GetData();
        return io::MemoryStream(std::span(data.GetPtrU8(), data.GetSize()));
    }

    template <typename T>
    T PopInteractiveInData() {
        auto stream = PopInteractiveInDataRaw();
        ASSERT(stream.GetSize() >= sizeof(T), Applets,
               "Not enough space ({} < {})", stream.GetSize(), sizeof(T));

        return stream.Read<T>();
    }

    void PushInteractiveOutDataRaw(const sized_ptr data) {
        controller.PushInteractiveOutData(new services::am::IStorage(data));
    }

    template <typename T>
    void PushInteractiveOutData(const T& data) {
        auto ptr = malloc(sizeof(T));
        memcpy(ptr, &data, sizeof(T));
        PushInteractiveOutDataRaw(sized_ptr(ptr, sizeof(T)));
    }

  private:
    services::am::LibraryAppletController& controller;

    std::thread* thread{nullptr};
    result_t result{RESULT_SUCCESS};
};

} // namespace hydra::horizon::applets
