#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::HosBinder {
class IHOSBinderDriver;
}

namespace Hydra::Horizon::Services::ViSrv {

class IApplicationDisplayService : public ServiceBase {
  public:
  protected:
    void RequestImpl(Readers& readers, Writers& writers,
                     std::function<void(ServiceBase*)> add_service,
                     Result& result, u32 id) override;

  private:
    HosBinder::IHOSBinderDriver* hos_binder_driver = nullptr;
};

} // namespace Hydra::Horizon::Services::ViSrv
