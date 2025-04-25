#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IStorage;

class ILibraryAppletAccessor : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ILibraryAppletCreator)

    ILibraryAppletAccessor(AppletId id_, LibraryAppletMode mode_);

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Kernel::HandleWithId<Kernel::Event> state_changed_event;

    AppletId id;
    LibraryAppletMode mode;

    std::stack<IStorage*> in_data;

    // Commands
    void GetAppletStateChangedEvent(REQUEST_COMMAND_PARAMS);
    void Start(REQUEST_COMMAND_PARAMS);
    void PushInData(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
