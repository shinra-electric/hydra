#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services {

class HidServer : public ServiceBase {
  public:
    HidServer(Handle handle_) : ServiceBase(handle_) {}

    void Request(Kernel& kernel, u8* out_ptr, usize& out_size,
                 u8* in_ptr) override;
};

} // namespace Hydra::Horizon::Services
