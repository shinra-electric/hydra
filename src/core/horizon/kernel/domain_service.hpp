#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/macros.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Kernel {

class DomainService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(DomainService)

    void Request(REQUEST_PARAMS) override;

    handle_id_t AddObject(ServiceBase* object) {
        handle_id_t handle_id = object_pool.AllocateForIndex();
        object_pool.GetObjectRef(handle_id) = object;

        return handle_id;
    }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override { unreachable(); }

  private:
    Allocators::DynamicPool<ServiceBase*> object_pool;
};

} // namespace Hydra::Horizon::Kernel
