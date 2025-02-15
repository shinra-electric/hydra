#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystemProxy : public ServiceBase {
  public:
  protected:
    void RequestImpl(Readers& readers, Writers& writers,
                     std::function<void(ServiceBase*)> add_service,
                     Result& result, u32 id) override;

  private:
};

} // namespace Hydra::Horizon::Services::Fssrv
