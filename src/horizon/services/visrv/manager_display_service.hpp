#pragma once

#include "horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class IManagerDisplayService : public IApplicationDisplayService {
  public:
  protected:
    void RequestImpl(Readers& readers, Writers& writers,
                     std::function<void(ServiceBase*)> add_service,
                     Result& result, u32 id) override;

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
