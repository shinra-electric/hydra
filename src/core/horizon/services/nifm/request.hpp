#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Nifm {

class IRequest : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IRequest)

    IRequest();

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Kernel::HandleWithId<Kernel::Event> events[2];

    // Commands
    void GetRequestState(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(GetResult);
    void GetSystemEventReadableHandles(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Nifm
