#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IStorage;

class ILibraryAppletAccessor : public ServiceBase {
  public:
    ILibraryAppletAccessor(AppletId id_, LibraryAppletMode mode_);

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> state_changed_event;

    AppletId id;
    LibraryAppletMode mode;

    std::queue<sized_ptr> in_data;

    // Commands
    result_t GetAppletStateChangedEvent(OutHandle<HandleAttr::Copy> out_handle);
    result_t Start();
    result_t PushInData(ServiceBase* storage_);

    // Helpers
    template <typename T> T PopInData() {
        ASSERT(!in_data.empty(), Services, "No input data");
        const auto data = in_data.front();
        ASSERT(data.GetSize() >= sizeof(T), Services,
               "Not enough space ({} < {})", data.GetSize(), sizeof(T));
        in_data.pop();

        return *reinterpret_cast<T*>(data.GetPtr());
    }
};

} // namespace hydra::horizon::services::am
