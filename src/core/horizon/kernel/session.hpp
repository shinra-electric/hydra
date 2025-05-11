#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Kernel {

class Session : public Handle {
  public:
    Session(ServiceBase* service_) : service{service_} {}

    void Close();
    void Request(RequestContext& context);
    void Control(Hipc::Readers& readers, Hipc::Writers& writers);

    // Getters
    ServiceBase* GetService() const { return service; }

    // Setters
    void SetHandleId(handle_id_t handle_id_) { handle_id = handle_id_; }

  private:
    ServiceBase* service;

    handle_id_t handle_id{invalid<handle_id_t>()};
};

} // namespace Hydra::Horizon::Kernel
