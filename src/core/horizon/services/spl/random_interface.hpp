#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Spl {

class IRandomInterface : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IRandomInterface)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::Spl
