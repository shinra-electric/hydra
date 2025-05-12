#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Hid {

class IActiveVibrationDeviceList : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ActivateVibrationDevice);
};

} // namespace Hydra::Horizon::Services::Hid
