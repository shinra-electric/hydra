#pragma once

#include "core/horizon/services/const.hpp"
#include "core/horizon/services/hid/const.hpp"

namespace hydra::horizon::services::hid {

class IHidSystemServer : public IService {
  public:
    IHidSystemServer();

  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    kernel::Event* home_button_event;
    kernel::Event* sleep_button_event;

    // Commands
    result_t
    AcquireHomeButtonEventHandle(kernel::Process* process,
                                 OutHandle<HandleAttr::Copy> out_handle);
    result_t
    AcquireSleepButtonEventHandle(kernel::Process* process,
                                  OutHandle<HandleAttr::Copy> out_handle);
    STUB_REQUEST_COMMAND(ApplyNpadSystemCommonPolicy);
    result_t
    GetUniquePadsFromNpad(NpadIdType npad_id, i64* out_count,
                          OutBuffer<BufferAttr::HipcPointer> out_buffer);
    result_t EnableAppletToGetInput(u8 enable, u64 aruid);
};

} // namespace hydra::horizon::services::hid
