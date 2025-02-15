#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Am {

class IApplicationProxy : public ServiceBase {
  public:
    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
};

} // namespace Hydra::Horizon::Services::Am
