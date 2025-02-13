#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services {

class ServiceManager : public ServiceBase {
  public:
    virtual void
    Request(Writers& writers, Reader& reader,
            std::function<void(ServiceBase*)> add_service) override;

  private:
};

} // namespace Hydra::Horizon::Services
