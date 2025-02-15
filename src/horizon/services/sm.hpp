#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services {

class ServiceManager : public ServiceBase {
  public:
    virtual void
    Request(Readers& readers, Writers& writers,
            std::function<void(ServiceBase*)> add_service) override;

  private:
};

} // namespace Hydra::Horizon::Services
