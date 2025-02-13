#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class IManagerRootService : public ServiceBase {
  public:
    void Request(Writers& writers, Reader& reader,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
