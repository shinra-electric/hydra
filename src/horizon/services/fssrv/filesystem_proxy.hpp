#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystemProxy : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFileSystemProxy)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::Fssrv
