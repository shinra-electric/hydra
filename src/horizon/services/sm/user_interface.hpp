#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Sm {

class IUserInterface : public ServiceBase {
  public:
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::Sm
