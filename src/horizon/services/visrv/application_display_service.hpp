#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::HosBinder {
class IHOSBinderDriver;
}

namespace Hydra::Horizon::Services::ViSrv {

class IApplicationDisplayService : public ServiceBase {
  public:
    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
    HosBinder::IHOSBinderDriver* hos_binder_driver = nullptr;
};

} // namespace Hydra::Horizon::Services::ViSrv
