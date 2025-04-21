#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Socket {

class IClient : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IClient)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void RegisterClient(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(StartMonitoring);
    STUB_REQUEST_COMMAND(Socket);
    STUB_REQUEST_COMMAND(Connect);
    STUB_REQUEST_COMMAND(Close);
};

} // namespace Hydra::Horizon::Services::Socket
