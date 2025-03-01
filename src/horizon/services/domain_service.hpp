#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/macros.hpp"
#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services {

class DomainService : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(DomainService)

    void Request(REQUEST_PARAMS) override;

    HandleId AddObject(ServiceBase* object) {
        HandleId handle_id = object_pool.AllocateForIndex();
        object_pool.GetObjectRef(handle_id) = object;
        object->SetHandleId(handle_id);

        return handle_id;
    }

  private:
    Allocators::DynamicPool<ServiceBase*> object_pool;
};

} // namespace Hydra::Horizon::Services
