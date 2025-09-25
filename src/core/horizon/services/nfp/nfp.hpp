#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::nfp {

enum class PermissionLevel {
    Debug,
    User,
    System,
};

class INfp : public IService {
  public:
    INfp(PermissionLevel perm_level_);

  protected:
    kernel::Event* availability_change_event;

    // Commands
    result_t Initialize(u64 aruid, u64 zero,
                        InBuffer<BufferAttr::MapAlias> in_version_buffer);
    // TODO: is the buffer attr correct?
    result_t ListDevices(i32* out_count,
                         OutBuffer<BufferAttr::HipcPointer> out_buffer);
    result_t GetState(u32* out_state);
    result_t
    AttachAvailabilityChangeEvent(kernel::Process* process,
                                  OutHandle<HandleAttr::Copy> out_handle);

  private:
    PermissionLevel perm_level;
};

} // namespace hydra::horizon::services::nfp
