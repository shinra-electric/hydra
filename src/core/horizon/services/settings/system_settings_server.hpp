#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::settings {

enum class ColorSetId : i32 {
    BasicWhite,
    BasicBlack,
};

class ISystemSettingsServer : public ServiceBase {
  public:
    // HACK
    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetFirmwareVersion(OutBuffer<BufferAttr::HipcPointer> out_buffer);
    result_t GetColorSetId(ColorSetId* out_id);
    // TODO: buffer attrs
    result_t
    GetSettingsItemValue(InBuffer<BufferAttr::HipcPointer> in_name_buffer,
                         InBuffer<BufferAttr::HipcPointer> in_item_key_buffer,
                         u64* out_size,
                         OutBuffer<BufferAttr::MapAlias> out_buffer);
    result_t GetDeviceNickName(OutBuffer<BufferAttr::MapAlias> out_buffer);
};

} // namespace hydra::horizon::services::settings
