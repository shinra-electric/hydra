#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Am {

class IStorage;

class ILibraryAppletAccessor : public ServiceBase {
  public:
    ILibraryAppletAccessor(AppletId id_, LibraryAppletMode mode_);

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    Kernel::HandleWithId<Kernel::Event> state_changed_event;

    AppletId id;
    LibraryAppletMode mode;

    std::stack<IStorage*> in_data;

    // Commands
    result_t GetAppletStateChangedEvent(OutHandle<HandleAttr::Copy> out_handle);
    result_t Start();
    result_t PushInData(ServiceBase* storage_);
};

} // namespace Hydra::Horizon::Services::Am
