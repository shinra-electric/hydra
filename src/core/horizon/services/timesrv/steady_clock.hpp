#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

class ISteadyClock : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ISteadyClock)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::TimeSrv
