#pragma once

#include "core/horizon/services/const.hpp"
#include "core/horizon/services/i2c/const.hpp"

namespace hydra::horizon::services::i2c {

class IManager : public IService {
  public:
    // HACK
    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t OpenSession(RequestContext* ctx, I2cDevice device);
    result_t HasDevice(I2cDevice device, bool* out_has);
};

} // namespace hydra::horizon::services::i2c
