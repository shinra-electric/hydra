#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services {

class DomainService : public ServiceBase {
  public:
    DomainService(Handle handle_) : ServiceBase(handle_) {}

    void Request(Kernel& kernel, Writer& writer, u8* in_ptr) override;

    Handle AddObject(ServiceBase* object) {
        Handle handle = object_pool.size();
        object_pool.push_back(object);

        return handle;
    }

  private:
    std::vector<ServiceBase*> object_pool;
};

} // namespace Hydra::Horizon::Services
