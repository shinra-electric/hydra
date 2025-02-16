#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::HosBinder {

class IHOSBinderDriver : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IHOSBinderDriver)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    void TransactParcel(REQUEST_COMMAND_PARAMS);
    void AdjustRefcount(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::HosBinder
