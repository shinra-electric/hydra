#pragma once

#include "core/horizon/hid.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::hid {

class IHidSystemServer : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ApplyNpadSystemCommonPolicy);
    result_t
    GetUniquePadsFromNpad(::hydra::horizon::hid::NpadIdType npad_id,
                          i64* out_count,
                          OutBuffer<BufferAttr::HipcPointer> out_buffer);
};

} // namespace hydra::horizon::services::hid
