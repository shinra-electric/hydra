#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Spl {

class IGeneralInterface : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IGeneralInterface)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetConfig(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Spl
