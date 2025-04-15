#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Socket {

class IClient : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IClient)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void RegisterClient(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(StartMonitoring);
};

} // namespace Hydra::Horizon::Services::Socket
