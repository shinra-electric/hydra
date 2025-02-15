#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

struct ParcelData {
    u32 unknown0;
    u32 unknown1;
    u32 binder_id;
    u32 unknown2[3];
    u64 str;
    u64 unknown3;
};

struct Parcel {
    u32 data_size;
    u32 data_offset;
    u32 objects_size;
    u32 objects_offset;
};

class DisplayServiceBase : public ServiceBase {
  protected:
    // Commands
    void CmdCreateStrayLayer(REQUEST_PARAMS_WITH_RESULT);
};

} // namespace Hydra::Horizon::Services::ViSrv
