#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IHidServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;
};

} // namespace Hydra::Horizon::Services::Hid
