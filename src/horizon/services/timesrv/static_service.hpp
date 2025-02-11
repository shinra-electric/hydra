#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

class IStaticService : public ServiceBase {
  public:
    void Request(Writers& writers, u8* in_ptr,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
    void CreateService(u32 id, std::function<void(ServiceBase*)> add_service);
};

} // namespace Hydra::Horizon::Services::TimeSrv
