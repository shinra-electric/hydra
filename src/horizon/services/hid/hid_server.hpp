#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;
};

} // namespace Hydra::Horizon::Services::Hid
