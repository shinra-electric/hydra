#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Hid {

class HidServer : public ServiceBase {
  public:
    HidServer(Handle handle_) : ServiceBase(handle_) {}

    void Request(Kernel& kernel, Writer& writer, Writer& move_handles_writer,
                 u8* in_ptr) override;
};

} // namespace Hydra::Horizon::Services::Hid
