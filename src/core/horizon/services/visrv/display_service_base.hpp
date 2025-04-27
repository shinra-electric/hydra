#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

struct ParcelData {
    u32 unknown0;
    u32 unknown1;
    u32 binder_id;
    u32 unknown2[3];
    u64 str;
    u64 unknown3;
};

class DisplayServiceBase : public Kernel::ServiceBase {
  protected:
    // Commands
    void CreateStrayLayer(REQUEST_COMMAND_PARAMS);
    void SetLayerVisibility(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::ViSrv
