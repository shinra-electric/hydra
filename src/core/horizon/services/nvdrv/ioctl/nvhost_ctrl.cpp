#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostCtrl, DEFINE_IOCTL_TABLE_ENTRY(0x00, 0x1b, GetConfig))

void NvHostCtrl::GetConfig(GetConfigData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    // const char* config = "UNKNOWN";
    // std::memcpy(&data.value, config, std::strlen(config) + 1);
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
