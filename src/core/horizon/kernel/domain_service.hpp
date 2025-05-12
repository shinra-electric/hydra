#pragma once

#include "core/horizon/kernel/handle_pool.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Kernel {

class DomainService : public ServiceBase {
  public:
    void Request(RequestContext& context) override;

    handle_id_t AddSubservice(ServiceBase* service) {
        return subservice_pool.Add(service);
    }

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override {
        unreachable();
    }

  private:
    DynamicHandlePool<ServiceBase> subservice_pool;
};

} // namespace Hydra::Horizon::Kernel
