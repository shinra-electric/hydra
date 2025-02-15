#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;
};

} // namespace Hydra::Horizon::Services::Hid
