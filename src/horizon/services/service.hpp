#pragma once

#include "common.hpp"

namespace Hydra::Horizon::Services {

class ServiceBase {
  public:
    virtual void Request(u8* out_data, usize& out_size, u32 id) = 0;
};

} // namespace Hydra::Horizon::Services
