#include "core/horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(
    ChannelBase,
    DEFINE_IOCTL_TABLE_ENTRY(0x47, 0x14, SetUserData, 0x15, GetUserData)
        DEFINE_IOCTL_TABLE_ENTRY(0x48, 0x01, SetNvMapFd, 0x08, SubmitGpfifo,
                                 0x09, AllocObjCtx, 0x0b, ZCullBind, 0x0c,
                                 SetErrorNotifier, 0x0d, SetPriority, 0x17,
                                 GetErrorNotification, 0x18, AllocGpfifoEx,
                                 0x1a, AllocGpfifoEx))

void ChannelBase::SetUserData(SetUserDataData& data, NvResult& result) {
    user_data = data.data;
}

void ChannelBase::GetUserData(GetUserDataData& data, NvResult& result) {
    data.data = user_data;
}

void ChannelBase::SetNvMapFd(SetNvMapFdData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: what's the purpose of knowing the nvmap fd?
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
