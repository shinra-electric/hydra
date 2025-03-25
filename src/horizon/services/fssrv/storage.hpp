#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IStorage : public ServiceBase {
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    virtual void Read(REQUEST_COMMAND_PARAMS) = 0;
};

} // namespace Hydra::Horizon::Services::Fssrv
