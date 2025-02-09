#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Time {

class StaticService : public ServiceBase {
  public:
    StaticService(Handle handle_) : ServiceBase(handle_) {}

    void Request(Kernel& kernel, Writer& writer, Writer& move_handles_writer,
                 u8* in_ptr) override;

  private:
    void CreateService(Kernel& kernel, Writer& move_handles_writer, u32 id);
};

} // namespace Hydra::Horizon::Services::Time
